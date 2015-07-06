#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>

#define GER_LEV 2
#define SEC 5

//Il processo iniziale (P0) deve creare 2 processi figli
//P1 e P2

//I processi P1 e P2 devono eseguire rispettivamente i
//comandi COM1 e COM2, rispettando il seguente vincolo:
//COM1 va eseguito soltanto al termine
//dell'esecuzione con successo di COM2.

//Ciascuno dei figli, in caso di fallimento della primitiva
//exec (usata per eseguire COM1 o COM2), dovrà
//notificare tale evento al padre P0, e successivamente
//terminare.

//Comunque vada, dopo N secondi (N > 0) dalla sua
//creazione, il processo P1 dovra` terminare la propria
//esecuzione.

void handleStatus();
void terminateSon1(int);
void notifyFather(int);
void notifySon1(int);

int pidF1; //var globale per la terminazione dopo N secondi di P1

int main(int argc, char** argv) {

	signal(SIGUSR1, notifyFather);
	signal(SIGALRM, terminateSon1);

	int pidP, pidFs[GER_LEV], pidN, i, N, status;

	pidP = getpid();

	//printf("%d", pidP);

	N = atoi(argv[1]);	//secondi esecuzione!

	for (i = 0; i < GER_LEV; ++i) {

		pidFs[i] = fork();

		if(i == 0) {

			pidF1 = pidFs[i];
			alarm(N);

		}

		if (pidFs[i] > 0) {

			//fluisci

		} else if (pidFs[i] == 0) {

			if (i == 0) {

				signal(SIGUSR1, notifySon1); //per la notifica di successo da parte del fratello dell' exec del nipote
				signal(SIGUSR2, notifySon1); //per la notifica di uscita da parte del fratello causa anomalia exec nipote

				//attendi fratello
				printf("Primo fratello %d: mi pongo in attesa del secondo fratello!\n", getpid());
				pause();

				//sleep(15); //a verifica dell' ultima condizione!

				execlp(argv[i+2], argv[i+2], (char *)0);
				printf("Primo fratello %d: Errore esecuzione primo comando! Notifico Padre!\n", getpid());
				sleep(2);

				printf("Primo fratello %d: Invio notifica a padre %d\n", getpid(), pidP);

				sleep(10);
				kill(pidP, SIGUSR1);
				exit(EXIT_FAILURE);


			} else if (i == 1) {

				pidN = fork();
				//creazione ulteriore figlio (NIPOTE) per eseguire secondo comando

				if (pidN == 0) {

					//nipote esegue comando
					execlp(argv[i+2], argv[i+2], (char *)0);
					//perror("Errore esecuzione secondo processo!");
					exit(EXIT_FAILURE);

				} else if (pidN > 0) {

					//figlio raccoglie stato nipote e invia segnale al fratello!
					wait(&status);

					if (WIFEXITED(status)) {

						if (WEXITSTATUS(status) == EXIT_SUCCESS) {

							printf("Secondo fratello %d: Esecuzione nipote %d andata a buon fine! Procedo con il risveglio di mio fratello!\n", getpid(), pidN);
							kill(pidFs[i-1], SIGUSR1);
							exit(EXIT_SUCCESS);

						} else if (WEXITSTATUS(status) == EXIT_FAILURE) {

							printf("Secondo fratello %d: Esecuzione nipote %d fallita! Procedo con la notificazione a mio padre e dico a mio fratello di terminare!\n", getpid(), pidN);

							sleep(2); //per dare il tempo di mettere in pausa il fratello e stabilire la gestione dei segnali!

							kill(pidP, SIGUSR1);
							kill(pidFs[i-1], SIGUSR2);

							exit(EXIT_FAILURE);

						}

					}

				}

			} else {

				perror("Errore fork!!");
				exit(EXIT_FAILURE);

			}
		}
	}

	for (i = 0; i < GER_LEV; i++) {

		handleStatus();

	}

	return EXIT_SUCCESS;

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
		printf("Padre %d: Processo %d terminato volontariamente con stato %d.\n", getpid(), pidAtteso, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("Padre %d: Processo %d terminato involontariamente causa segnale %d.\n", getpid(), pidAtteso, WTERMSIG(status));

}

void terminateSon1(int signo)
{

	printf("Padre %d: Scaduto tempo di esecuzione primo fratello! Terminazione in corso!\n", getpid());
	kill(pidF1, SIGKILL);

}

void notifyFather(int signo)
{

	printf("Padre %d: Fallita l'exec di uno dei miei figli!\n", getpid());
	return;

}

void notifySon1(int signo)
{

	if (signo == SIGUSR1) {

		printf("Primo fratello %d: Notifica a figlio1 di andare avanti!\n", getpid());
		return;

	} else if (signo == SIGUSR2) {

		printf("Primo fratello %d: Notifica a figlio1 di terminare!!\n", getpid());
		exit(EXIT_FAILURE);

	}

}
