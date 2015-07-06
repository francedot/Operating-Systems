/*
 ============================================================================
 Name        : Esercizio2.c
 Author      : Francesco
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

#define MAX_DIM_STR 200
#define SORGENTI_PATH "/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione2/Assets/Sorgenti/"
#define EXE_PATH "/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione2/Assets/Eseguibili/"
#define COMP_ERR -1
#define EXE__ERR -2
#define SIGN_ERR -3

typedef char stringa[MAX_DIM_STR];

void buildSorgNameAbs(stringa ,stringa);
void buildExeNameAbs(stringa, stringa);
void handleStatus();
void handleSons(int);

int main(int argc, char** argv)
{
	int i, pidN, pidF, statusN;

	stringa nomeFileSorgAbs;
	stringa nomeFileExeAbs;

	for (i = 1; i < argc; i++) {

		//costruzione nome assoluto file binario
		buildSorgNameAbs(argv[i], nomeFileSorgAbs);

		//costruzione nome assoluto file eseguibile
		buildExeNameAbs(argv[i], nomeFileExeAbs);

		pidF = fork();

		if (pidF == 0) { //ENTRA IL FIGLIO

			pidN = fork();

			if (pidN == 0) { //ENTRA IL NIPOTE

				execlp("gcc", "gcc", nomeFileSorgAbs, "-o", nomeFileExeAbs, (char *)0);
				printf("Errore compilazione file %s.", argv[i]);
				perror("\n");
				exit(COMP_ERR);

			} else if (pidN > 0){ //ENTRA IL FIGLIO

				wait(&statusN);
				//gestione stato ed eventualmente esecuzione eseguibile!

				if (WIFEXITED(statusN)) {

					if (WEXITSTATUS(statusN) < 0)
						exit(COMP_ERR);	//non provare affatto a eseguire il file!
					else {

						printf("F %d: Processo N %d terminato volontariamente con stato %d.\n", getpid(), pidN, WEXITSTATUS(statusN));
						printf("Provo a eseguire file %s:\n", nomeFileExeAbs);
						execl(nomeFileExeAbs, nomeFileExeAbs, (char *)0);
						//se errore nel comando di esecuzione!
						//printf("%s\n", nomeFileExeAbs);
						fprintf(stderr, "Errore esecuzione file %s: %s\n", nomeFileExeAbs, strerror(errno));
						exit(EXE__ERR);

					}

				} else if (WIFSIGNALED(statusN)) {

					printf("F %d: Processo N %d terminato involontariamente causa segnale %d.\n\n", getpid(), pidN, WTERMSIG(statusN));
					exit(SIGN_ERR);

				}

			}

		} else {

			//LASCIA ANDARE IL PADRE!

		}

	}

	handleSons(argc-1);

	return EXIT_SUCCESS;
}

void buildSorgNameAbs(stringa nomeFileSorg, stringa nomeFileSorgAbs) //(in, out)
{
	strcpy(nomeFileSorgAbs, SORGENTI_PATH);
	strcat(nomeFileSorgAbs, nomeFileSorg);
}

void buildExeNameAbs(stringa nomeFileSorg, stringa nomeFileExeAbs) //(in, out)
{
	stringa temp;
	int termIndex;
	char* pch;

	strcpy(nomeFileExeAbs, EXE_PATH);
	strcpy(temp, nomeFileSorg);

	pch = strrchr(nomeFileSorg, '.');
	termIndex = pch - nomeFileSorg; //trova l'indice del punto nella stringa
	temp[termIndex] = '\0';

	strcat(nomeFileExeAbs, temp);
}

void handleStatus()
{
	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	} else if (WIFEXITED(status)) {

		printf("PO %d: Processo F %d terminato volontariamente con stato %d.\n\n", getpid(), pidAtteso, WEXITSTATUS(status));

	}  else if (WIFSIGNALED(status)) {

		printf("PO %d: Processo F %d terminato involontariamente causa segnale %d.\n\n", getpid(), pidAtteso, WTERMSIG(status));

	}
}

void handleSons(int numF)
{
	int i;

	for (i = 0; i < numF; i++)
			handleStatus();
}
