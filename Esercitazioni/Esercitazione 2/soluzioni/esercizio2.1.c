#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int i, pid, status, n_children;
	n_children = atoi(argv[1]);
	for ( i=0; i<n_children; i++ ) {
		pid = fork();
		if ( pid == 0 ) { // Eseguito dai figli
				printf("sono il figlio %d\n", getpid());
				exit(0);
			}
		else if ( pid > 0 ) { // Eseguito dal padre
			printf("Padre %d: creato figlio con PID %d\n", getpid(), pid);
		}
		else {
			perror("Fork error:");
			exit(1);
		}
	}
	for ( i=0; i<n_children; i++ ) {
		pid = wait(&status);
		if ( (char)status==0 ) { // terminazione volontaria
				printf("Terminato il figlio %d con stato %d\n", pid, status>>8);
				
			}
		else { // terminazione involontaria
				printf("Terminato il figlio %d per segnale %d \n", pid, (char) status);
				
			}
	}

return 0;	
}
