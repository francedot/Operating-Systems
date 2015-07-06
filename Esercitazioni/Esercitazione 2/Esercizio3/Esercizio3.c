/*
 ============================================================================
 Name        : Esercizio3.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define FORK_ERR -1
#define EXE__ERR -2
#define MAX_DIM_STR 200
#define EXE_PATH "/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione2/Assets/Eseguibili/"

typedef char stringa[MAX_DIM_STR];

//Sintassi di invocazione:
//-:$ ./eseguiComandi K COM1 COM2 ... COMN


//Significato degli argomenti:
//• eseguiComandi è il nome del file eseguibile associato al
//programma.
//• COM1,COM2,...,COMN sono N stringhe che rappresentano
//il nome di un file (per semplicita`, si supponga che il
//direttorio di appartenenza del file COM sia nel PATH)‫‏‬
//• K e` un valore intero positivo (minore di N)‫‏‬


//Specifiche:

//Il processo iniziale (P0) deve mettere in esecuzione
//gli N comandi passati come argomenti, secondo la
//seguente logica:
//•  i primi K comandi passati come argomenti
//dovranno essere eseguiti in parallelo da
//altrettanti figli di P0
//•  Al termine dei primi K processi, i restanti N-K
//comandi dovranno essere eseguiti in sequenza da
//altrettanti figli e/o nipoti di P0

void handleStatusP();
void handleStatusS();
void handleSonsP(int);
void handleSonsS(int);
void recursiveSequence(char**, int, int);

int main(int argc, char** argv) //N = argc-2
{
	int pidF, i;
	int K = atoi(argv[1]);
	stringa nomeFileExeAbs;

	//primi k comandi da essere eseguiti in // da k figli di P0
	//si attende la terminazione dei k processi... wait del padre
	//--> gestibile anche con raccolta dello stato--> handleSons()
	//terminati i k processi bisogna eseguire gli n-k comandi rimanenti
	//in sequenza--> creazione di una gerarchia con radice P0 di livello
	//n-k+1 (versione iterativa o ricorsiva)

	printf("Esecuzione comandi in parallelo. L'ordine può variare di volta in volta causa scheduler!\n");

	for (i = 2; i < K+2/*shift di 2*/; i++) {

		pidF = fork();

		if (pidF == 0) {

			strcpy(nomeFileExeAbs, EXE_PATH);
			strcat(nomeFileExeAbs, argv[i]);

			//printf("Provo a eseguire file %s:\n", nomeFileExeAbs);
			execl(nomeFileExeAbs, nomeFileExeAbs, (char *)0);
			fprintf(stderr, "Errore esecuzione file %s: %s\n", nomeFileExeAbs, strerror(errno));
			exit(EXE__ERR);

		} else if(pidF > 0) {

			//LASCIA ANDARE IL PADRE

		} else {

			fprintf(stderr, "Errore fork al giro %d!%s", i-1, strerror(errno));
			exit(FORK_ERR);

		}

	}

	handleSonsP(K); //garantisce che si siano conclusi i k processi

	pidF = fork();

	if (pidF == 0) {

		printf("\nEsecuzione comandi in serie. L'ordine è quello che ho deciso per la ricorsione! \nProssimo Processo ha affidata la ricorsione!\n");
		// ??? errore compilatore?

		recursiveSequence(argv, argc-1, K); //la affido al figlio F0
		exit(EXIT_SUCCESS);

	}
	else if (pidF > 0) {

		handleStatusS(); //PO aspetta F0 prima di chiudersi!
		exit(EXIT_SUCCESS);
	}

}

void handleStatusP()
{
	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	}

	//questa volta stati non gestiti! Incasinano l' output per via dell'ordine casuale di esecuzione!
//	else if (WIFEXITED(status)) {
//
//		printf("PO %d: Processo F %d terminato volontariamente con stato %d.\n\n", getpid(), pidAtteso, WEXITSTATUS(status));
//
//	}  else if (WIFSIGNALED(status)) {
//
//		printf("PO %d: Processo F %d terminato involontariamente causa segnale %d.\n\n", getpid(), pidAtteso, WTERMSIG(status));
//
//	}
}

void handleStatusS()
{
	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	}

	else if (WIFEXITED(status)) {

		printf("P %d: Processo F %d terminato volontariamente con stato %d.\n\n", getpid(), pidAtteso, WEXITSTATUS(status));

	}  else if (WIFSIGNALED(status)) {

		printf("P %d: Processo F %d terminato involontariamente causa segnale %d.\n\n", getpid(), pidAtteso, WTERMSIG(status));

	}
}

void handleSonsP(int numF)
{
	int i;

	for (i = 0; i < numF; i++)
			handleStatusP();
}

void handleSonsS(int numF)
{
	int i;

	for (i = 0; i < numF; i++)
			handleStatusS();
}

void recursiveSequence(char** argv, int argc_m_1, int K)
{
	int pidF;
	stringa nomeFileExeAbs;

	strcpy(nomeFileExeAbs, EXE_PATH);
	strcat(nomeFileExeAbs, argv[argc_m_1]);
	//condizionne limite!
	if (argc_m_1 >= K+2) { //6!=6

		pidF = fork();

		if (pidF > 0) { //padre esegue comando!

			handleStatusS();
			execl(nomeFileExeAbs, nomeFileExeAbs, (char *)0);
			fprintf(stderr, "Errore esecuzione file %s: %s\n", nomeFileExeAbs, strerror(errno));
			exit(EXE__ERR);

		} else if (pidF == 0) { //figlio esegue un altra fork! o meglio richiama la funzione!

			recursiveSequence(argv, argc_m_1-1, K);
			exit(EXIT_SUCCESS); //far uscire i figli al ritorno!

		}

	} else {

		//REACHED END

	}
}











