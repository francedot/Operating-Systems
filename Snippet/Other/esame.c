//Si deve elaborare un file di testo contenente un numero di righe, ciascuna al massimo di 80
//caratteri. Dati due caratteri (c1, e c2) si vuole calcolare il numero di righe che iniziano con un
//carattere diverso da ‘#” e che contengono almeno 15 occorrenze del carattere x, dove x viene
//calcolato come segue:
//• x = c1, se nel file di ingresso vi è un numero pari di righe che iniziano con il carattere ‘#’,
//• x= c2, in caso contrario.
//Si realizzi questo comportamento scrivendo un programma C (esame) invocabile utilizzando la
//seguente interfaccia:
//$ esame fileIn fileOut c1 c2
//● fileIn è il nome assoluto del file di ingresso.
//● fileOut è il nome assoluto del file che dovrà contenere l’output del programma: se già
//esistente, l’output dovrà essere scritto in APPEND sul file esistente.
//● c1 e c2 sono due qualsiasi caratteri.
//L’implementazione deve essere strutturata in maniera che si rispettino le seguenti ulteriori
//specifiche.
//Il processo principale P0, dopo aver verificato la correttezza degli argomenti, deve generare due
//processi figli P1 e P2, i quali si dividerano il compito di elaborare il file di ingresso e scrivere il
//risultato sul file di uscita come segue:
//● P2 dovrà leggere il contenuto di fileIn, e comunicare, man mano che la lettura avanza,
//una riga alla volta al processo P1, ECCETTO le righe che iniziano con il carattere ‘#’.
//● P1, dovrà esaminare le righe ricevute da P2 per calcolare, alla fine, il numero di righe che
//contengono almeno 15 occorrenze del carattere x; successivamente scriverà su fileOut
//la stringa “L'input contiene <n> righe con almeno 15 occorrenze del carattere x”, dove
//<n> è il risultato calcolato.
//La definizione del valore di x dovrà essere realizzata come segue:
//● Alla lettura di una riga che inizia con ‘#’ il processo P2 dovrà inviare il segnale
//SIGUSR1 al processo P1.
//● P1 dovrà conteggiare il numero totale di volte in cui ha ricevuto SIGUSR1, in maniera
//tale da assegnare a x il valore corretto.
//Nella scrittura del programma, si assuma un modello affidabile dei segnali (segnali consecutivi
//non sono mai persi) e si assuma che l’handler non venga resettato dopo la gestione di ogni
//segnale.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define STR_MAX_DIM 50
#define LABEL '#'

typedef char string[STR_MAX_DIM];

char x;
int countL = 0;

int contaRigheWL(char*);
void leggiRiga(int, string);
void labelNotify(int signo);
int fineFile(int);

int main(int argc, char** argv)
{

	int numRighe, pidF[2], pidP, i, numRigheWL = 0, pipe21[2];

	if (argc != 5) {

		perror("Errore sintassi comando!\n");
		exit(EXIT_FAILURE);

	}

	//NO AI FINI DI DEBUGGING
//	if (argv[1][0] != '/' || argv[2][0] != '/') {
//
//		perror("Nome file non assoluto!\n");
//		exit(EXIT_FAILURE);
//
//	}

	pidP = getpid();

	/*** CREAZIONE GERARCHIA LIVELLO 2 ***/
	for (i = 0; i < 2; i++) {

		pidF[i] = fork();

		if (pidF[i] > 0) {//P0



		} else if (pidF[i] == 0) {

			if (i == 0) {//P1

				int numC1 = 0, numC2 = 0, i = 0, numRigheMag15C1 = 0, numRigheMag15C2 = 0, fdOut;
				char c;
				string riga;
				string output;

				signal(SIGUSR1, labelNotify);

				//CREAZIONE PIPE DI COMUNICAZIONE TRA P2 E P1
				if (pipe(pipe21) < 0) {

					fprintf(stderr, "P2 %d: Errore creazione pipe21!\n", getpid());
					exit(EXIT_FAILURE);

				}

				if (close(pipe21[1])) {

					fprintf(stderr, "P2 %d: Errore chiusura pipe21 lato scrittura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				while ((read(pipe21[0], &c, sizeof(char))) > 0) {

					if (c == argv[3]) { //c1

						numC1++;

					} else if (c == argv[4]) { //c2

						numC2++;

					} else if (c == '\n') {

						if (numC1 >= 15)
							numRigheMag15C1++;

						if (numC1 >= 15)
							numRigheMag15C2++;

						numC1 = 0;
						numC2 = 0;

					}

				}

				if (close(pipe21[0])) {

					fprintf(stderr, "P1 %d: Errore chiusura pipe21 lato lettura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				if (countL % 2) {

					sprintf(output, "L'input contiene %d righe con almeno 15 occorrenze del carattere x", numRigheMag15C1);


				} else {

					sprintf(output, "L'input contiene %d righe con almeno 15 occorrenze del carattere x", numRigheMag15C2);

				}

				if ((fdOut = open(argv[1], O_WRONLY | O_APPEND)) < 0) {

					fprintf(stderr, "P1 %d: Errore apertura fdOut!\n", getpid());
					exit(EXIT_FAILURE);

				}

				write(fdOut, output);

				if ((fdOut = close()) < 0) {

					fprintf(stderr, "P1 %d: Errore chiusura fdOut!\n", getpid());
					exit(EXIT_FAILURE);

				}

				//“L'input contiene <n> righe con almeno 15 occorrenze del carattere x”, dove <n>

				exit(EXIT_SUCCESS);


			} else if (i == 1) {//P2
			//● P2 dovrà leggere il contenuto di fileIn, e comunicare, man mano che la lettura avanza,
			//una riga alla volta al processo P1, ECCETTO le righe che iniziano con il carattere ‘#’.

				int fdIn;
				char c;
				string riga;

				if (close(pipe21[0])) {

					fprintf(stderr, "P2 %d: Errore chiusura pipe21 lato lettura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				//LETTURA FILE DI INPUT

				if ((fdIn = open(argv[1], O_RDONLY)) < 0) {

					fprintf(stderr, "P2 %d: Errore apertura fdIn!\n", getpid());
					exit(EXIT_FAILURE);

				}

				while (!fineFile(fdIn)) {

					leggiRiga(fdIn, riga);
					printf("P2 %d: Letta riga %s.\n", getpid(), riga);

					if (riga[0] == LABEL) {

						kill(pidF[0], SIGUSR1);

					} else {

						write(pipe21[1], riga, strlen(riga)*sizeof(char));

					}

				}

				if (close(pipe21[1])) {

					fprintf(stderr, "P2 %d: Errore chiusura pipe21 lato scrittura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				if (close(fdIn) < 0) {

					fprintf(stderr, "P2 %d: Errore chiusura fdIn!\n", getpid());
					exit(EXIT_FAILURE);

				}

				exit(EXIT_SUCCESS);

			}

		} else {



		}


	}

	printf("numrighe=%d\n", numRighe);
	return 0;
}

int contaRigheWL(char* fileIn)
{
	int fdIn, numRighe = 0;
	string riga;

	if ((fdIn = open(fileIn, O_RDONLY)) < 0) {

		fprintf(stderr, "P0 %d: Errore apertura fdIn!\n", getpid());
		exit(EXIT_FAILURE);

	}

	while (!fineFile(fdIn)) {

		leggiRiga(fdIn, riga);
		printf("P0 %d: Letta riga %s.\n", getpid(), riga);

		if (riga[0] != LABEL) {

			numRighe++;

		}

	}

	if (close(fdIn) < 0) {

		fprintf(stderr, "P0 %d: Errore chiusura fdIn!\n", getpid());
		exit(EXIT_FAILURE);

	}

	return numRighe;

//		int fdIn, numRighe = 0;
//		char c;
//
//		if ((fdIn = open(fileIn, O_RDONLY)) < 0) {
//
//			perror("Errore lettura righe!\n");
//			exit(EXIT_FAILURE);
//
//		}
//
//		while (read(fdIn, &c, sizeof(char)) > 0) {
//
//			if (c == '\n') {
//
//				numRighe++;
//			}
//
//		}
//
//		if (close(fdIn) < 0) {
//
//			perror("Errore chiusura fdIn!\n");
//			exit(EXIT_FAILURE);
//
//		}
}

void leggiRiga(int fdIn, string riga)
{
	char c;
	int i = 0;

	while (read(fdIn, &c, sizeof(char)) > 0 && c != '\n') {

		riga[i++] = c;

	}

	riga[i] = '\0';

}

void labelNotify(int signo)
{
	signal(SIGUSR1, &labelNotify);

	countL++;

//	if ((countL % 2) == 0) {
//
//		x = argv[3];
//
//	} else {
//
//		x = argv[3]
//
//	}

	return;

}

int fineFile(int fdIn)
{
	char c;
	if (read(fdIn, &c, sizeof(char)) == 0) {

		return 1;

	} else {

		lseek(fdIn, -(sizeof(char)), SEEK_CUR);
		return 0;
	}
}
