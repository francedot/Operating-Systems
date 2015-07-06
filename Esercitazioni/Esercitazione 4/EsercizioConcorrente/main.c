#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_STRING_LENGTH 128

/*
 * ALCUNE NOTE:
 * IO Pointer separati, P2 e P1 procedono concorrentemente.
 * LEGGI ATTENTAMENTE le note sulla sincronizzazione sparse qua e la
 * nel codice: danno importanti informazioni sul perché questa soluzione
 * abbia numerosi problemi, e come risolverli.
 */

/***********************/
/* HANDLER DEI SEGNALI */
/***********************/
/* Handler dei segnali gestiti da P0*/
void handler_correzioni(int signo);

/* Handler dei segnali gestiti da P1*/
void handler_maggiore(int signo);
void handler_fine_lettura(int signo);


/**************************/
/* DICHIARAZIONE FUNZIONI */
/**************************/
void wait_child();
void processo_p2(char *inputfile, int p1_pid);
void processo_p1(char *inputfile);
void print_output(char *outputfile);

/*********************/
/* VARIABILI GLOBALI */
/*********************/
int maggiore; /* 0: A >= B ; 1: A < B ; */
int fine_lettura; /*0: lettura in corso; 1: lettura finita */
int num_correzioni;

int main(int argc, char* argv[])
{
    int pid[2], i;

    if (argc != 3)
    {
        fprintf(stderr, "Numero di argomenti errato\n");
        fprintf(stderr, "Usage:\n\t%s file_in file_out\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Inizializzo le variabili globali a valori noti*/
    num_correzioni = 0;
    fine_lettura = 0;
    maggiore = -1;

    /*Registrazione handler padre*/
    signal(SIGUSR1, &handler_correzioni);

    for (i=0; i<2; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            fprintf(stderr, "P0: Errore nella fork");
            exit(EXIT_FAILURE);
        }
        else if (pid[i] == 0)
        {
            if (i==0) /*P1*/
            {
                processo_p1(argv[1]);
                exit(EXIT_SUCCESS);
            }
            else if (i==1)
            {
                processo_p2(argv[1], pid[0]);
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            printf("P0: creato figlio P%d con PID %d\n", i+1, pid[i]);
        }
    }

    /* Attesa della fine dei figli */
    for(i=0;i<2;i++)
    {
        wait_child();
    }

    /* Scrittura dell'output sul file di destinazione*/
    print_output(argv[2]);

    return 0;
}

void print_output(char *outputpath)
{
    int fd, written;
    char buf[MAX_STRING_LENGTH];
    fd = creat(outputpath, 00640);
    if (fd < 0)
    {
        sprintf(buf, "Impossibile aprire il file %s", outputpath);
        perror(buf);
        exit(EXIT_FAILURE);
    }

    written = write(fd, &num_correzioni, sizeof(int));
    if (written < 0)
    {
        sprintf(buf, "Impossibile scrivere sul file %s", outputpath);
        perror(buf);
        exit(EXIT_FAILURE);
    }

    return;
}

void handler_correzioni(int signo)
{
    /* Dovrebbe sempre essere SIGUSR1 a meno di errori*/

    /* Incremento il numero di correzioni effettuate*/
    /*
     * ATTENZIONE: Cosa succede se al momento dell'invocazione
     * ASINCRONA del segnale, un'altra istruzione stava leggendo
     * num_correzioni?
     * Nel caso specifico il problema è minore in quanto
     * num_correzioni viene solo letta una volta che i processi
     * figlio sono terminati (non arrivano ulteriori segnali da loro)
     * CASO GENERALE MOLTO CRITICO: soluzione pulita con uso di altri
     * strumenti di sincronizzazione per  l'esecuzione atomica di operazioni
     */

    num_correzioni++;
}

void processo_p2(char *inputfile, int p1)
{
    int fd, nread, read_buf[2];
    /* Do il tempo a P1 di impostare i suoi handler dei segnali.
    Vedi nota sulle slide riguardo all'uso delle sleep */
    sleep(1);

    fd = open(inputfile, O_RDONLY);
    if ( fd < 0 )
    {
        perror("P2: errore nell'apertura del file di input");
        /* Segnalo a p1 la fine della lettura */
        close(fd);
        kill(p1, SIGALRM);
        exit(EXIT_FAILURE);
    }

    while ( (nread = read(fd, read_buf, 2*sizeof(int))) > 0 ) /*Fino a EOF*/
    {
        int seek_val;
        if(read_buf[0] >= read_buf[1]) /* A >= B */
        {
            kill(p1, SIGUSR1);
        }
        else
        {
            kill(p1, SIGUSR2);
        }
        /*Sposto avanti il cursore per la prossima lettura
          devo saltare la entry corrispondente a C */
        seek_val = lseek(fd, sizeof(int), SEEK_CUR);
        if ( seek_val < 0 )
        {
            perror("P2: errore nella lettura del file di input");
            close(fd);
            kill(p1, SIGALRM);
            exit(EXIT_FAILURE);
        }


        /*
         * Voglio evitare di mandare segnali a P1
         * troppo velocemente.
         */
        sleep(1);
    }
    /* Gestione errori in read */
    if (nread < 0)
    {
        perror("P2: errore nella lettura del file di input");
        close(fd);
        kill(p1, SIGALRM);
        exit(EXIT_FAILURE);
    }

    /*Fine elaborazione, segnalo a P1*/
    close(fd);
    kill(p1, SIGALRM);

    return;

}

void processo_p1(char *inputfile)
{
    int fd, nrw, buf[2], sk;

    signal(SIGALRM, &handler_fine_lettura);
    signal(SIGUSR1, &handler_maggiore);
    signal(SIGUSR2, &handler_maggiore);

    fd = open(inputfile, O_RDWR);
    if ( fd < 0 )
    {
        perror("P1: errore nell'apertura del file di input");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /*
     * ATTENZIONE: fine_lettura viene modificata da un handler
     * di segnali che viene eseguito in maniera ASINCRONA.
     * Che succede ad esempio se accade la seguente successione
     * di operazioni?:
     * 1. la condizione del while viene valutata TRUE (fine_lettura == 0)
     * 2. Prima della pause arriva il segnale SIGARLM, che imposta
     *    fine_lettura ad 1
     * 3. Il controllo ritrona al programma che esegue la pause(), ma non
     *    arriverà mai più un nuovo segnale :(
     *
     * Gli strumenti visti fin ora non sono sufficienti per risolvere il
     * problema: servono meccanismi per accesso atomico a sezioni critiche
     * di codice.
     */
    while (!fine_lettura)
    {
        /* Aspetto il prossimo segnale*/
        pause();

        /*Il codice dell'handler avrà scritto un valore nella variabile
          globale maggiore*/
        switch(maggiore)
        {
            case 0:
                /* A > B */
                /* Posizione dell'IO pointer invariata*/
                nrw = read(fd, buf, sizeof(int));
                if(nrw < 0)
                {
                    perror("P1: errore in lettura");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                /*Mi sposto su C*/
                sk = lseek(fd, sizeof(int), SEEK_CUR);
                if (sk < 0 )
                {
                    perror("P1: errore in lettura");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                break;
            case 1:
                /*Mi sposto su B*/
                sk = lseek(fd, sizeof(int), SEEK_CUR);
                if (sk < 0 )
                {
                    perror("p1: errore in lettura");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                nrw = read(fd, buf, sizeof(int));
                if(nrw < 0)
                {
                    perror("P1: errore in lettura");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                /* Può arrivere qui se fine lettura viene impostata dopo
                   essere entrati nel blocco del ciclo */
                /*In tal caso non continuo il codice del blocco,
                  ma rinizio il giro del ciclo*/
                continue;
        }
        /* Leggo C e lo metto nel secondo elemento di buf*/
        nrw = read(fd, buf+1, sizeof(int));
        if(nrw < 0) {
            perror("P1: errore in lettura");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if ( buf[0] != buf[1])
        {
            /* Sposto indietro il cursore*/
            sk = lseek(fd, -sizeof(int), SEEK_CUR);
            if (sk < 0 )
            {
                perror("p1: errore in lettura");
                close(fd);
                exit(EXIT_FAILURE);
            }
            /* Srivo il primo elemento di buf su C*/
            nrw = write(fd, buf, sizeof(int));
            if(nrw < 0)
            {
                perror("P1: errore in scrittura");
                close(fd);
                exit(EXIT_FAILURE);
            }
            /*Segnalo la correzione*/
            kill(getppid(), SIGUSR1);
        }
    }

    close(fd);
    return;
}

void handler_maggiore(int signo)
{
    /*
     * Imposto la variabile globale maggiore a seconda della segnalazione
     * arrivata.
     * PROBLEMA SERIO: cosa succede se P2 esegue più velocemente di P1?
     * Potrebbe mandare più di un SIGUSR prima che P1 faccia in tempo
     * ad effettuare le correzioni.
     * In quel caso, con questa soluzione, la variabile maggiore verrebbe
     * sovrascritta ed il suo valore perso.
     * Come risolviamo? Alcune idee:
     * 1) Sincronizzazione più stretta (P2 aspetta che P1 effettui le correzioni)
     * 2) Buffer più largo (non una variabile maggiore, ma un array modellato
     *    come una coda) -> Problema sincronizzazione produttore/consumatore
     *
     * Nel secondo caso, sono comunque necessari meccanismi di mutua esclusione
     * per prevenire corse critiche.
     *
     * Soluzione barbara: ad ogni segnalazione P2 fa una sleep, per dare il tempo
     * a P1 di gestire la sua segnalazione. E' una implementazione brutta e concettualmente
     * sbagliata dell'idea 1 (chi garantisce che P1 faccia in tempo in un secondo a fare
     * il suo dovere?), ma rapida nel codificare.
     * Inoltre, la sleep limita di molto la concorrenza: se per P1 fosse necessario molto
     * meno di un secondo per fare il suo lavoro?
     * Ai fini dell'esame va bene purché si specifichi esplicitamente
     * il motivo per cui la si fa, ed i problemi a cui si va incontro se le temporizzazioni
     * non sono rispettate.
     */
    switch(signo)
    {
        case SIGUSR1:
            maggiore = 0;
            break;
        case SIGUSR2:
            maggiore = 1;
            break;
        default:
            fprintf(stderr, "P1: segnale inaspettato\n");
            exit(EXIT_FAILURE);
    }
    return;
}

void handler_fine_lettura(int signo)
{
    fine_lettura = 1;
    maggiore = -1;
    return;
}


void wait_child() {
    int pid_terminated,status;

    pid_terminated=wait(&status);
    if (pid_terminated < 0)
    {
        fprintf(stderr, "%d\n", getpid());
        perror("P0: errore in wait");
        exit(EXIT_FAILURE);
    }
    if(WIFEXITED(status))
    {
        printf("P0: terminazione volontaria del figlio %d con stato %d\n",
                pid_terminated, WEXITSTATUS(status));
        if (WEXITSTATUS(status) == EXIT_FAILURE)
        {
            fprintf(stderr, "P0: errore nella terminazione del figlio pid_terminated\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(WIFSIGNALED(status))
    {
        fprintf(stderr, "P0: terminazione involontaria del figlio %d a causa del segnale %d\n",
                pid_terminated,WTERMSIG(status));
        exit(EXIT_FAILURE);
    }
}
