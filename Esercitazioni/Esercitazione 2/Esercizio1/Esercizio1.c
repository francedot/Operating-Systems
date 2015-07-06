/*
 ============================================================================
 Name        : Esercizio1.c
 Author      : Francesco
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void handleStatus();

int main(int argc, char *argv[]) {

	int i, pid, n_children;
	i=1;
	n_children = atoi(argv[1]);

	for ( i=0; i<n_children; i++ ) {
		pid = fork();

		if ( pid == 0 ) { // Eseguito dai figli

			exit(0);

		} else if ( pid > 0 ) { // Eseguito dal padre

			printf("Padre: %d. Child created with PID %d\n", getpid(), pid);
			handleStatus();

		} else {

			perror("Fork error:");
			exit(1);
		}
	}

	exit(EXIT_SUCCESS);
}

void handleStatus()
{

	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");
		exit(-1);
	}

	if (WIFEXITED(status))
		printf("Processo %d terminato volontariamente con stato %d.\n", pidAtteso, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("Processo %d terminato involontariamente causa segnale %d.\n", pidAtteso, WTERMSIG(status));

}
