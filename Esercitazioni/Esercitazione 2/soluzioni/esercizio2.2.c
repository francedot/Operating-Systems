#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_STRING_LENGTH 255
#define GCC_PATH "/usr/bin/gcc"

void usage(char* prog_name)
{
    fprintf(stderr,"Usage:\n\t%s file1.c [file2.c ... ]\n", prog_name);
}

int main (int argc, char* argv[])
{
    int num_files, i;
    /*
     * Buffer temporaneo dove, per comodità scrivo i messaggi di
     * errore prima di stamparli 
     */
    char buff[MAX_STRING_LENGTH];
  
    if ( ! argc > 1 ) /*Almeno un file da compilare*/
    {
        fprintf(stderr,"Numero di argomenti non valido\n");
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    num_files = argc - 1;

    for( i=0; i<num_files; i++ )
    {
        int pid = fork();
        if ( pid > 0 ) /* Processo padre*/
        {
            printf("Padre %d: creato figlio %d con pid: %d\n", getpid(), i, pid);
        }
        else if ( pid == 0 ) /*Processo figlio*/
        {
            /* 
             * Per nostra convenzione l'output è uguale al nome del sorgente
             * senza il ".c"
             */
            char executable[MAX_STRING_LENGTH]; 
            char* source_file = argv[i+1];
            int source_name_len = strlen(source_file);
            /* "Costruisco" il nome dell'eseguibile */
            strncpy( executable, "./", 2 );
            strncpy( executable+2, source_file, source_name_len-2 );
            executable[source_name_len] = '\0';
			
            /*
             * Genera un processo nipote e poi aspetta la sua
             * terminazione, prima di lanciare l'eseguibile da lui prodotto
             */
            pid = fork();
            if ( pid > 0 ) /*Processo figlio */
            {
                int status, terminated_pid;
				printf("Figlio %d: attendo la compilazione di %s\n", i, source_file);
                terminated_pid = wait(&status);
                if ( terminated_pid < 0 ) /* Error condition */
                {
                    sprintf(buff, "Figlio %d: errore durante l'attesa:",i);
                    perror(buff);
                    exit(EXIT_FAILURE);
                }
                
                /*Controllo se il nipote ha terminato con successo la compilazione*/
                if ( WEXITSTATUS(status) == EXIT_SUCCESS )
                {
                    /* Posso avviare l'esecuzione */
                    execl( executable, executable, (char*)0 );
                    /*
                     * Se arrivo qui la execl ha fallito 
                     */
                    sprintf(buff,"Figlio %d: impossibile eseguire l'eseguibile %s\n",
                            i, executable);
                    perror(buff);
                    exit(EXIT_FAILURE);
                }
		else /* Errore di compilazione*/
                {
                    fprintf(stderr, "Figlio %i: errore di compilazione da parte di mio nipote. Esco\n", i);
                    exit(EXIT_FAILURE);
                }


            }
            else if ( pid == 0 ) /* Processo nipote */
            {
		printf("Nipote di %d: compilo il sorgente %s in %s\n",i, source_file, executable);
                execl (GCC_PATH, GCC_PATH, "-o", executable, source_file, (char*)0 );
                sprintf(buff, "Nipote di %d: impossibile compilare il sorgente %s",
                       i, source_file);
                perror( buff );
                exit(EXIT_FAILURE); 
            }
            else 
            {
                sprintf(buff, "Figlio %d: impossibile fare fork:",i);
                perror(buff);
                exit(EXIT_FAILURE);
            }

        }
        else 
        {
            perror("Padre: impossibile fare fork:");
            exit(EXIT_FAILURE);
        }
    }

    /*
     * Controllo dello stato di terminazione degli N processi figlio.
     * Chi esegue questa porzione di codice? 
	 * Solo il padre perchè gli altri processi fanno una execl (che sostituisce il codice). 
	 * Se la execl fallisce, fanno una exit, quindi solo il padre arriva qui.
	 * Perchè non eseguo questo controllo prima, dentro il for precendente? 
	 * Se sposto questa wait nel for precedente il padre P0 crea ogni figlio esecutore e 
	 * poi si mette in attesa della sua terminazione prima di creare il successivo. 
	 * L'esercizio chiedeva invece di sequanzializzare il meno possibile. 
	 * Ciò che può essere fatto in parallelo deve esser fatto in parallelo. 
     */
    for ( i=0; i<num_files; i++ )
    {
                int status, terminated_pid;
                terminated_pid = wait(&status);
                if ( terminated_pid < 0 ) /* Error condition */
                {
                    perror("Padre: errore durante l'attesa");
                    continue;
                }
                
                if ( ! WEXITSTATUS(status) == EXIT_SUCCESS )
                {
                    fprintf(stderr,"Padre: qualcuno dei figli (PID=%d) non è terminato con successo. Sono molto deluso.\n",
                            terminated_pid);
                }
    }

    return EXIT_SUCCESS;
}

