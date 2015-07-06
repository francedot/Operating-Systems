#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define COPY_PATH "/home/francesco/Desktop/f_InCopy"

int fineFile = 0;

void copy(char* file1[], char* file2[])
{
	if (fork() == 0) {

		execlp("cp", "cp", file1, file2, (char *)0);
		perror("Errore copia!");
		exit(EXIT_FAILURE);

	}

	sleep(2);
	return;
}

void F1EOFNotification(int signo)
{
	printf("F1 %d: Ricevuto segnale di fine file!\n", getpid());
	fineFile = 1;
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

int main(int argc, char** argv)
{
	int pidF[2], fd_21[2], fd_10[2], fdIn, i, sum = 0, val, readRes;

	if (pipe(fd_10) < 0) {

		printf("Errore creazione Pipe 10");

	}

	if (pipe(fd_21) < 0) {

		printf("Errore creazione Pipe 21");

	}

	/**PER AVERE UNA COPIA DI BACKUP!*/
	copy(argv[1], COPY_PATH);
	chmod(COPY_PATH, 00777);

	for (i = 0; i < 2; i++) {

		pidF[i] = fork();

		if (pidF[i] < 0) {

			perror("Errore fork!");

		} else if (pidF[i] == 0) {

			if (i == 0) { //F1

				int C, maxAB;
				signal(SIGUSR1, &F1EOFNotification);
				close(fd_10[0]); //chiusura lato di lettura pipe 10
				close(fd_21[1]); //chiusura lato di scrittura pipe 21

				/*F1 DEVE SIA LEGGERE CHE SCRIVERE*/
				fdIn = open(COPY_PATH, O_RDWR);

				while (!fineFile) {

					if(lseek(fdIn, 2*sizeof(int), SEEK_CUR) < 0) {

						fprintf(stderr, "F1 %d: Errore scorrimento IO Pointer!\n", getpid());
						exit(EXIT_FAILURE);

					}

					readRes = read(fdIn, &C, sizeof(int));
					if (readRes != sizeof(int) && readRes != 0) {

						fprintf(stderr, "F1 %d:Errore lettura C da file fdIn!", getpid());
						exit(EXIT_FAILURE);

					}

					printf("F1 %d: Letto da fdIn %d.\n", getpid(), C);

					readRes = read(fd_21[0], &maxAB, sizeof(int));
					if (readRes != sizeof(int) && readRes != 0) {

						fprintf(stderr, "F1 %d:Errore lettura da pipe 21!", getpid());
						exit(EXIT_FAILURE);

					}

					if (C != maxAB) {

						/*VOGLIO SCRIVERE NELLA POSIZIONE DI C*/
						if (lseek(fdIn, -(sizeof(int)), SEEK_CUR) < 0) {

							fprintf(stderr, "F1 %d:Errore scorrimento IO Pointer!", getpid());
							exit(EXIT_FAILURE);

						}

						if (!fineFile) {
							write(fdIn, &maxAB, sizeof(int));
							write(fd_10[1], &maxAB, sizeof(int));
						}

					}

				}

				close(fd_10[1]);
				close(fd_21[0]);
				close(fdIn);

				exit(EXIT_SUCCESS);


			} else if (i == 1) { //F2

				int AB[2], maxAB;

				close(fd_21[0]); //chiudo lato di lettura!
				close(fd_10[0]);
				close(fd_10[1]);

				fdIn = open(COPY_PATH, O_RDONLY);

				while (read(fdIn, AB, 2*sizeof(int)) == 2*sizeof(int)) {

					printf("F2 %d: Letto %d %d.\n", getpid(), AB[0], AB[1]);
					maxAB = (AB[0] > AB[1]) ? AB[0] : AB[1];

					if (write(fd_21[1], &maxAB, sizeof(int)) != sizeof(int)) {

						fprintf(stderr, "F2 %d: Errore scrittura su pipe 21!", getpid());
						exit(EXIT_FAILURE);

					} else {

						if(lseek(fdIn, sizeof(int), SEEK_CUR) < 0) {

							fprintf(stderr, "F2 %d: Errore scorrimento IO Pointer!\n", getpid());
							exit(EXIT_FAILURE);

						}
					}
				}

				printf("F2 %d: Fine file!\n", getpid());

				sleep(1);
				kill(pidF[0], SIGUSR1);

				close(fd_21[1]);
				close(fdIn);
				exit(EXIT_SUCCESS);

			}

		} else if (pidF[i] > 0) {

				//fluisci!

		}

	}

	close(fd_10[1]);
	close(fd_21[0]);
	close(fd_21[1]);

	while ((readRes = read(fd_10[0], &val, sizeof(int))) != 0) {

		printf("P0 %d: Ricevuta correzione %d.\n", getpid(), val);
		sum += val;

	}

	for (i = 0; i < 3; i++)
		handleStatus();

	printf("Ecco la somma: %d.\n", sum);

	return EXIT_SUCCESS;
}
