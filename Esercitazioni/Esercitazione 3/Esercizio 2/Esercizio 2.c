#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_CYCLE 500000
#define GER_LEV 2
#define SEC 5
#define FILESTAMPE "/home/francesco/Dropbox/Ingegneria/Corsi Attuali/Sistemi Operativi T/Esercizi/Esercitazioni/Esercitazione3/Esercizio 2/stampePidP.txt"

//Realizzare una variante dell’ esercizio 2:

//Il padre P0, mentre il figlio esegue, continua la propria
//attivita` (cioe` non si pone in attesa di P1 e P2), e
//stampa il suo PID ripetutamente.

//Tuttavia, non appena ognuno dei suoi figli terminerà, P0
//dovrà tempestivamente raccogliere e stampare il suo
//stato di terminazione.
//vedi gestione del segnale SIGCHLD

void handleStatus();
void terminateSon1(int);
void notifyFather(int);
void notifySon1(int);
void childrenHandler(int);

int pidF1; //var globale per la terminazione dopo N secondi di P1

int main(int argc, char** argv) {

	//2 stream differenti per le operazioni di scrittura del pid e esecuzione comandi!
	//1) Esecuzione file su sdout
	//2) Scrittura pidP su file di testo FILESTAMPE

	FILE* fp = fopen(FILESTAMPE, "wt");

	signal(SIGUSR1, notifyFather); //notifica al padre che uno dei due figli ha fallito l'exec!
	signal(SIGALRM, terminateSon1); //notifica al padre che P1 ha impiegato più di N secondi!

	signal(SIGCHLD, childrenHandler); //notifica al padre che un figlio ha terminato! comportamento dato a tutti i processi!

	int pidP, pidFs[GER_LEV], pidN, i, N, status;

	pidP = getpid();

	N = atoi(argv[1]);	//secondi esecuzione!

	for (i = 0; i < MAX_CYCLE; i++) {

		fprintf(fp, "Padre: Il mio pid è %d.\n", pidP);

		if (i < GER_LEV) {

			pidFs[i] = fork();

//			if(i == 0) {
//
//				pidF1 = pidFs[i];
//				alarm(N);
//
//			}

			if (pidFs[i] > 0) {

				//fluisci

			} else if (pidFs[i] == 0) {

				if (i == 0) {

					signal(SIGUSR1, notifySon1); //per la notifica di successo da parte del fratello dell' exec del nipote
					signal(SIGUSR2, notifySon1); //per la notifica di uscita da parte del fratello causa anomalia exec nipote

					//attendi fratello
					printf("Primo fratello %d figlio di %d: mi pongo in attesa del secondo fratello!\n", getpid(), getppid());
					pause();

					signal(SIGCHLD, childrenHandler);

					//eseguo il comando!
					execlp(argv[i+2], argv[i+2], (char *)0);

					//se errore
					printf("Primo fratello %d: Errore esecuzione primo comando! Notifico Padre!\n", getpid());
					printf("Primo fratello %d: Invio notifica a padre %d\n", getpid(), pidP);
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

						signal(SIGCHLD, SIG_IGN);

						wait(&status);

						if (WIFEXITED(status)) {

							signal(SIGCHLD, childrenHandler);

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
	}

	pause();
	sleep(2);

	fclose(fp);
	return EXIT_SUCCESS;

}

void handleStatus()
{

	sleep(1);

	int pidAtteso, status;
	pidAtteso = wait(&status);

//	if (pidAtteso < 0) {
//
//		perror("Errore Attesa Figlio!");
//		exit(-1);
//	}

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

void childrenHandler(int signo)
{

	handleStatus();

}
