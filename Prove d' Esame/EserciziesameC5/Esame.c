//Sintassi di invocazione: Esame C N

//Significato degli argomenti:
//•  Esame è il nome del file eseguibile associato al programma.
//•  N e` un intero non negativo.
//•  C e` una stringa che rappresenta il nome di un file eseguibile (per
//semplicita`, si supponga che il direttorio di appartenenza del file
//C sia nel PATH).

//Specifiche:
//Il processo iniziale (P0) deve creare 1 processo figlio P1 che, a sua
//volta crea un proprio figlio P2 . Si deve quindi ottenere una gerarchia di
//3 processi: P0 (padre), P1 (figlio) e P2 (nipote).
//•  Il processo P2, una volta creato, passa ad eseguire il comando C.
//•  Il processo P1 , dopo aver generato P2 , deve mettersi in attesa di uno
//dei 2 eventi seguenti:
//1. la ricezione di un segnale dal padre, oppure
//2. la terminazione di P2.
//Nel primo caso (ricezione di un segnale da P0) P1 termina forzatamente
//l’esecuzione di P2 e poi termina.
//Nel secondo caso (terminazione di P2), P1 invia un segnale al padre P0 e
//successivamente termina trasferendo a P0 il pid di P2
//•  Il processo P0, dopo aver generato P1, entra in un ciclo nel quale, ad
//ogni secondo, incrementa un contatore K; se K raggiunge il valore N, P0
//invia un segnale al figlio P1 e termina. Nel caso in cui P0 riceva un segnale
//da P1 durante l’esecuzione del ciclo, prima di terminare dovra` stampare
//lo stato di terminazione del figlio e successivamente terminare.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_DIM_STR 50
typedef char string[50];

#define READ_ACCESS 04
#define WRITE_ACCESS 02
#define EXECUTE_ACCESS 01
#define EXISTENCE 00

//VAR GLOBALI:

int pidP0, pidP1, pidP2, pipe10[2];

void accesso(char** path, int modo)
{
	if (access(path, modo) < 0) {

		perror("Errore: file non esistente o non si ha il diritto scelto su quel file.\n");
		exit(EXIT_FAILURE);

	}

	return;
}

void creaPipe(int _pipe[])
{
	if (pipe(_pipe) < 0) {

		fprintf(stderr, "P0 %d: errore creazione pipe!\n", getpid());
		exit(EXIT_FAILURE);

	}
}

void chiudi(int fd)
{
	if (close(fd) < 0) {

		fprintf(stderr, "P %d: errore chiusura fd!\n", getpid());
		exit(EXIT_FAILURE);

	}

}

void handlerP0(int signo)
{

	//P1 mi ha inviato SIGUSR1 per avvertirmi di leggere dalla pipe e di controllare
	//il mio stato di terminazione
	int pidAtteso, status;

	printf("P0 %d: ricevuto SIGUSR1 da %d. Leggo da pipe10 e attendo terminazione P1. N.B: non conosco P2=%d\n", getpid(), pidP1, pidP2);

	read(pipe10[0], &pidP2, sizeof(int));
	printf("P0 %d: ora so che il pid di P2 è %d\n", getpid(), pidP2);

	chiudi(pipe10[0]);

	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	} else if (WIFEXITED(status)) {

		printf("P0 %d: Processo P1 %d terminato volontariamente con stato %d.\n", getpid(), pidAtteso, WEXITSTATUS(status));

	}  else if (WIFSIGNALED(status)) {

		printf("P0 %d: Processo P1 %d terminato involontariamente causa segnale %d.\n", getpid(), pidAtteso, WTERMSIG(status));

	}

	exit(EXIT_SUCCESS);

}

void handlerP1(int signo)
{
	if (signo == SIGTERM) {

		printf("P1 %d: ricevuto SIGTERM da %d. Uccido P2.\n", getpid(), pidP0);
		kill(pidP2, SIGKILL);

	} else if (signo == SIGCHLD) {

		printf("P1 %d: ricevuto SIGCHLD da %d. Notifico P0 e scrivo su pipe10.\n", getpid(), pidP2);
		//invio segnale di avvertimento
		kill(pidP0, SIGUSR1);

		//scrivo su pipe
		write(pipe10[1], &pidP2, sizeof(int));
		dchiudi(pipe10[1]);

	}
}

int main(int argc, char** argv)
{

	int N;

	if (argc != 3) {

		perror("Errore: numero parametri.");
		exit(EXIT_FAILURE);

	}

	//numero valore max per p0
	N = atoi(argv[2]);

	if (N < 0) {

		perror("Errore: N negativo.");
		exit(EXIT_FAILURE);

	}

	accesso(argv[1], EXECUTE_ACCESS);

	/** PIPE PER COMUNICAZIONE P0 E P1 **/
	creaPipe(pipe10);

	//creazione gerarchia 2 livelli
	pidP0 = getpid();
	printf("P0 %d: sono P0 %d.\n", pidP0, pidP0);

	pidP1 = fork();
	if (pidP1 > 0) { //P0

		int k = 0;

		printf("P0 %d: ho creato il figlio P1 %d.\n", pidP0, pidP1);

		signal(SIGUSR1, &handlerP0);
		chiudi(pipe10[1]); //chiudo lato di scrittura pipe10

		while (1) {

			if (k == N) {

				kill(pidP1, SIGTERM);
				exit(EXIT_SUCCESS);

			}

			sleep(1);
			k++;

		}

		exit(EXIT_SUCCESS);

	} else if (pidP1 == 0) { //P1

		chiudi(pipe10[0]); //chiudo lato di lettura pipe10

		printf("P1 %d: sono stato creato da P0 %d.\n", getpid(), pidP0);

		//creo 1 altro livello
		pidP1 = getpid(); //sovrascrivo con quello che mi interessa
		pidP2 = fork();

		if (pidP2 > 0) { //P1

			printf("P1 %d: ho creato il figlio P2 %d.\n", pidP1, pidP2);
			signal(SIGTERM, &handlerP1);
			signal(SIGCHLD, &handlerP1);

			//P1: Mi metto in attesa di SIGTERM o della terminazione di P2
			pause();

			exit(EXIT_SUCCESS);

		} else if (pidP2 == 0) { //P2

			printf("P2 %d: sono stato creato da P1 %d.\n", getpid(), pidP1);

			/** ESECUZIONE COMANDO **/
			//faccio durare comando qualche secondo
			sleep(7);
			execl(argv[1], argv[1], (char *)0);
			fprintf(stderr, "P2 %d: errore exec.\n", getpid());
			exit(EXIT_FAILURE);

		} else if (pidP2 <  0) { //errore

			perror("Errore fork2.");
			exit(EXIT_FAILURE);

		}

		exit(EXIT_SUCCESS);

	} else if (pidP1 <  0) { //errore

		perror("Errore fork1.");
		exit(EXIT_FAILURE);

	}

}


