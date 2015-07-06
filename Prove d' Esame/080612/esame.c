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
//segnale. --> ERGO ANCHE NON VA COME DEVE NON TI SCORAGGIARE XK DI PER SE IL MODELLO DEI SEGNALI IN LINUX E' NON AFFIDABILE


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define STR_MAX_DIM 100
#define LABEL '#'

typedef char string[STR_MAX_DIM];

char x;
int countL = 0;

int contaRigheWL(char*);
void leggiRiga(int, string);
void labelNotify(int signo);
int fineFile(int);

void handleStatus()
{
	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	} else if (WIFEXITED(status)) {

		printf("PO %d: Processo F %d terminato volontariamente con stato %d.\n", getpid(), pidAtteso, WEXITSTATUS(status));

	}  else if (WIFSIGNALED(status)) {

		printf("PO %d: Processo F %d terminato involontariamente causa segnale %d.\n", getpid(), pidAtteso, WTERMSIG(status));

	}
}

void handleChildren(int numF)
{
	int i;

	for (i = 0; i < numF; i++)
			handleStatus();
}

int main(int argc, char** argv)
{

	int pidF[2], pidP, i, pipe21[2];

	/*** VERIFICA SINTASSI ***/
	if (argc != 5) {

		perror("Errore sintassi comando!\n");
		exit(EXIT_FAILURE);

	}

	pidP = getpid();
	printf("P0 %d: sono il padre.\n", pidP);

	/*** CREAZIONE STRUTTURA PIPE PER COMUNICAZIONE TRA P2 E P1 ***/
	//La creo nel padre poichè deve essere usata dai 2 figli!
	if (pipe(pipe21) < 0) {

		fprintf(stderr, "P0 %d: Errore creazione pipe21!\n", pidP);
		exit(EXIT_FAILURE);

	}


	/*** CREAZIONE GERARCHIA LIVELLO 2 ***/
	for (i = 0; i < 2; i++) {

		pidF[i] = fork();

		if (pidF[i] > 0) {//P0

			//FLUISCI

		} else if (pidF[i] == 0) {

			/*** FIGLIO1 ***/
			if (i == 0) {//P1

				int numC1 = 0, numC2 = 0, numRigheMag15C1 = 0, numRigheMag15C2 = 0, fdOut;
				char c;
				string output;

				signal(SIGUSR1, labelNotify);
				printf("P1 %d: sono il figlio 1.\n", getpid());

				if (close(pipe21[1])) {

					fprintf(stderr, "P1 %d: Errore chiusura pipe21 lato scrittura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				while ((read(pipe21[0], &c, sizeof(char))) > 0) {

					printf("P1 %d: ricevuto dalla pipe il carattere %c.\n", getpid(), c);

					if (c == argv[3][0]) { //c1

						numC1++;
						printf("P1 %d: trovato %c nella riga. Numero attuale occorrenze=%d.\n", getpid(), c, numC1);

					} else if (c == argv[4][0]) { //c2

						numC2++;
						printf("P1 %d: trovato %c nella riga. Numero attuale occorrenze=%d.\n", getpid(), c, numC2);

					} else if (c == '\0') {

						printf("P1 %d: Fine riga\n", getpid());

						if (numC1 >= 15)
							numRigheMag15C1++;

						if (numC2 >= 15)
							numRigheMag15C2++;

						//nuovo conteggio
						numC1 = 0;
						numC2 = 0;

					}

				}

				if (close(pipe21[0])) {

					fprintf(stderr, "P1 %d: Errore chiusura pipe21 lato lettura!\n", getpid());
					exit(EXIT_FAILURE);

				}

				printf("P1 %d: valore corrente countL=%d.\n", getpid(), countL);
				if ((countL % 2) == 0) {

					sprintf(output, "L'input contiene %d righe con almeno 15 occorrenze del carattere %c.\n", numRigheMag15C1, argv[3][0]);


				} else {

					sprintf(output, "L'input contiene %d righe con almeno 15 occorrenze del carattere %c.\n", numRigheMag15C2, argv[4][0]);

				}

				if ((fdOut = open(argv[2], O_CREAT | O_WRONLY | O_APPEND, 00777)) < 0) {

					fprintf(stderr, "P1 %d: Errore apertura fdOut!\n", getpid());
					exit(EXIT_FAILURE);

				}

				printf("P1 %d: scrivo risultato su fdOut.\n", getpid());
				write(fdOut, output, sizeof(char) * strlen(output));

				if (close(fdOut) < 0) {

					fprintf(stderr, "P1 %d: Errore chiusura fdOut!\n", getpid());
					exit(EXIT_FAILURE);

				}

				exit(EXIT_SUCCESS);


			} else if (i == 1) {//P2

				int fdIn, dimRiga;
				string riga;

				printf("P2 %d: sono il figlio 2.\n", getpid());

				sleep(1); //aspetto creazione pipe
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
					dimRiga = strlen(riga);
					printf("P2 %d: Letta riga %s di lunghezza %d.\n", getpid(), riga, dimRiga);


					if (riga[0] == LABEL) {

						sleep(1);// aspetto dichiarazione signal di figlio1;
						kill(pidF[0], SIGUSR1);

					} else {

						//prima di inviarlo aggiungo un terminatore di fine rifa
						printf("P2 %d: invio riga %s.\n", getpid(), riga);
						riga[dimRiga] = '\0';
						write(pipe21[1], riga, dimRiga*sizeof(char)+1);

					}

				}

				//broken pipe?
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

		} else { //problema fork



		}


	}

	//P0 chiude la pipe perchè non gli serve
	if (close(pipe21[0])) {

		fprintf(stderr, "P0 %d: Errore chiusura pipe21 lato lettura!\n", pidP);
		exit(EXIT_FAILURE);

	}
	if (close(pipe21[1])) {

		fprintf(stderr, "P0 %d: Errore chiusura pipe21 lato scrittura!\n", pidP);
		exit(EXIT_FAILURE);

	}


	handleChildren(2);
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
	printf("P0 %d: ricevuto segnale di presenza riga con primo carattere #. countL=%d.\n", getpid(), countL);
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
