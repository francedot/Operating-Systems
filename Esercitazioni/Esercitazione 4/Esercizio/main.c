//Si realizzi un programma C che, usando le opportune system call
//unix, realizzi la seguente interfaccia:
//	./correggi f_in f_out
//•  f_in: path di un file binario esistente contenente
//N triplette di numeri interi, con N non noto a priori.
//•  f_out: path di un file non esistente nel filesystem

//NB: file binario ≠ file di testo.
//Quanti byte occupa “800667” su file binario? E su file di
//testo?

//Il programma deve realizzare il seguente comportamento
//•  Il processo padre (P0) deve generare due figli P1 e P2
//•  Il processo P2 deve
//•  Leggere i primi due interi (A,B) di ogni tripletta in
//file_in
//•  Al termine di ogni lettura deve segnalare a P1 quale (A o
//B) è il maggiore
//•  Letta l'ultima tripletta, comunicare a P1 il termine della
//sua elaborazione

//Il processo P1 deve:
//•  Reperire dal file l'intero maggiore (A o B) a seconda della
//segnalazione ricevuta da P2
//•  Leggere il valore di C e, nel caso in cui questo risultasse
//diverso dal massimo appena letto
//•  Scrivere il valore dell'intero maggiore al posto del relativo
//elemento C della tripletta
//•  Comunicare a P0 l'avvenuta correzione
//Il processo P0 deve
//•  Tener traccia del numero di correzioni effettuate
//•  Al termine dell'elaborazione dei figli, scrivere tale valore su
//file_out

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define GER_LEV 2
#define COPY_PATH "/home/francesco/Desktop/f_InCopy"

typedef int boolean;

boolean fine = 0;
int dimInt = sizeof(int);
int numCorr = 0;
int pidF[GER_LEV], pidP;
int maxAB;
int fdIn;

void wait_child()
{
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

void PCorrectionNotification(int signo)
{
	printf("Sono P %d: F1 %d mi ha detto che ha trovato un errore!\n", pidP, pidF[0]);
	numCorr++;
	return;
}

void F1EOFNotification(int signo)
{
	printf("Sono F1 %d: F2 %d mi ha detto che f_in è terminato!\n", pidF[0], pidF[1]);
	exit(EXIT_SUCCESS);
}

void F1MaxABNotification(int signo)
{
	read(fdIn, &maxAB, dimInt); //parte comune!

	if (signo == SIGUSR1) {

		printf("Sono F1 %d: F2 %d mi ha detto che A è maggiore!\n", pidF[0], pidF[1]);
		lseek(fdIn, dimInt, SEEK_CUR);

	} else {

		printf("Sono F1 %d: F2 %d mi ha detto che B è maggiore uguale!\n", pidF[0], pidF[1]);
		//sono apposto con l'I/O Pointer!

	}

	return;
}

void F2ProceedNotification(int signo)
{
	printf("Sono F2 %d: F1 %d mi ha detto di procedere con l'analisi!\n", pidF[1], pidF[0]);
	return;
}

void copy(char** file1, char** file2)
{
	if (fork() == 0) {

		execlp("cp", "cp", file1, file2, (char *)0);
		perror("Errore copia");
		exit(EXIT_FAILURE);

	}

	sleep(2); //rallento padre per dare la possibilità al figlio di copiare il file!
	return;
}

int main(int argc, char** argv)
{
	int i, fdIn, fdOut;

	if (argc != 3) {

		perror("Errore numero argomenti!");
		exit(EXIT_FAILURE);

	}

	//ai nostri fini creiamo una copia in lettura e scrittura del file di input! uso di cp in bin
	copy(argv[1], COPY_PATH); //c'è un altro figlio n.b
	chmod(COPY_PATH, 00777);

	fdIn = open(COPY_PATH, O_RDWR);

	if (fdIn < 0) {

		perror("Errore apertura file!");
		exit(EXIT_FAILURE);

	}

	pidP = getpid();
	signal(SIGUSR1, PCorrectionNotification); //avviso padre quando figlio1 esegue correzione!

	//creazione gerarchia
	for (i = 0; i < GER_LEV; i++) {

		pidF[i] = fork();

		if (pidF[i] < 0) {

			perror("Errore Fork!");
			exit(EXIT_FAILURE);

		} else if (pidF[i] == 0) {

			if (i == 0) { //Figlio1

				int C;

				signal(SIGALRM, F1EOFNotification);
				signal(SIGUSR1, F1MaxABNotification);
				signal(SIGUSR2, F1MaxABNotification);

				for(;;) {

					printf("Sono F1 %d arrivo prima della pause!\n", getpid());

					pause();

					read(fdIn, &C, dimInt);

					if (C != maxAB) {

						kill(pidP, SIGUSR1);

					}

				}


			} else if (i == 1) { //Figlio2

				int AB[2];
				signal(SIGUSR1, F2ProceedNotification);

				//printf("Sono F2 %d arrivo prima della read!\n", getpid());

				//leggo la coppia A e B fino alla fine del file!

				for(;;) {

					if (read(fdIn, AB, dimInt*2) != dimInt*2) {

						printf("read F2 non a buon fine");
						sleep(1);
						//si verifica se provo a leggere ancora dopo l'ultima tripletta!
						kill(pidF[0], SIGALRM); //imposto già le posizioni
						exit(EXIT_SUCCESS);

					} else {//letti correttamente

						printf("read F2 a buon fine");

						if (AB[0] > AB[1]) {

							lseek(fdIn, (-2)*dimInt , SEEK_CUR);
							kill(pidF[i-1], SIGUSR1);

						}

						else {

							lseek(fdIn, (-1)*dimInt , SEEK_CUR);
							kill(pidF[i-1], SIGUSR2);

						}

						pause(); //in attesa di conferma per continuare da parte di F1

					}

				}

			}

		} else if (pidF[i] > 0) {

			//fluisci

		}

	}

	sleep(3);

	for(i = 0;i < 3; i++)
		 wait_child();

	return EXIT_SUCCESS;
}

