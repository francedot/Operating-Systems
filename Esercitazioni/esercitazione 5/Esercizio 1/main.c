//•  Si realizzi un programma C che, usando le opportune system
//call unix, realizzi la seguente interfaccia:
//./correggi file_in file_out
//•  ﻿ file_out: nome di un file non esistente nel filesystem
//•  file_in: nome di un file binario esistente contenente N
//triplette di numeri interi, con N non noto a priori.
//
//Il programma deve realizzare il seguente comportamento:
//•  Il processo padre (P0) deve generare due figli P1 e P2
//•  Il processo P2 deve:
//•  Leggere i primi due interi (A,B) di ogni tripletta in
//file_in;
//•  Al termine della lettura dei primi due elementi di
//ogni tripletta, P2 deve comunicare a P1 il valore del
//maggiore;
//•  Letta l'ultima tripletta, segnalare a P1 il termine
//della sua elaborazione
//
//Il processo P1 deve
//•  Leggere il valore di C e, nel caso in cui questo
//risultasse diverso dal massimo comunicatogli da P2
//•  Scrivere il valore dell'intero maggiore al posto del
//relativo elemento C della tripletta
//•  Comunicare a P0 il valore corretto
//
//Il processo P0 deve
//•  Sommare i valori ricevuti da P1
//•  Al termine dell'elaborazione dei figli, scrivere tale
//valore su file_out


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define COPY_PATH "/home/francesco/Desktop/f_InCopy"

int fineFile = 0;

void copy(char* file1[], char* file2[])
{
	if (fork() == 0) {

		execlp("cp", "cp", file1, file2, (char *)0);
		perror("Errore copia");
		exit(EXIT_FAILURE);

	}

	sleep(2);
	return;
}

void F1EOFNotification(int signo)
{
	printf("Arrivata la fine del file!\n");
	fineFile = 1;
}

int main(int argc, char** argv)
{

	int pidF[2], fd_21[2], fd_10[2], fdIn, i, sum = 0, val;

	if (pipe(fd_10) < 0) {

			printf("Errore creazione Pipe 10");

		}

	//close(fd_10[1]); se lo chiudi qui fai danni!

	if (pipe(fd_21) < 0) {

		printf("Errore creazione Pipe 21");

	}

	copy(argv[1], COPY_PATH);
	chmod(COPY_PATH, 00777);

	fdIn = open(COPY_PATH, O_RDWR);

	for (i = 0; i < 2; i++) {

		pidF[i] = fork();

		if (pidF[i] < 0) {

			perror("Errore fork!");

		} else if (pidF[i] == 0) {

			if (i == 0) {

				int C, maxAB;
				signal(SIGUSR1, &F1EOFNotification);
				close(fd_10[0]); //chiusura lato di lettura pipe 10
				close(fd_21[1]); //chiusura lato di scrittura pipe 21

				while (!fineFile) {

//					if (fineFile)
//						break;
					if (read(fdIn, &C, sizeof(int)) != sizeof(int)) {
						fprintf(stderr, "F1 %d:Errore lettura da file fdIn!", getpid());
						exit(EXIT_FAILURE);
					}

					printf("F1 %d: Letto da fdIn %d.\n", getpid(), C);

					if (read(fd_21[0], &maxAB, sizeof(int)) != sizeof(int)) {
						fprintf(stderr, "F1 %d:Errore lettura da pipe 21!", getpid());
						exit(EXIT_FAILURE);
					}


					if (C != maxAB) {

						lseek(fdIn, -(sizeof(int)), SEEK_CUR);
						write(fdIn, &maxAB, sizeof(int));

						write(fd_10[1], &maxAB, sizeof(int));

					}

				}

				exit(EXIT_SUCCESS);


			} else if (i == 1) {

				int AB[2], maxAB;

				close(fd_21[0]); //chiudo lato di lettura!
				close(fd_10[0]);
				close(fd_10[1]);

				//sleep(2); //rallento!

				while (read(fdIn, AB, 2*sizeof(int)) == 2*sizeof(int)) {

					printf("F2 %d: Letto %d %d.\n", getpid(), AB[0], AB[1]);
					maxAB = (AB[0] > AB[1]) ? AB[0] : AB[1];

					if (write(fd_21[1], &maxAB, sizeof(int)) != sizeof(int)) {
						fprintf(stderr, "F2 %d: Errore scrittura su pipe 21!", getpid());
						exit(EXIT_FAILURE);
					}

				}

				//printf("F2 %d: Fine file!\n", getpid());
				kill(pidF[0], SIGUSR1);
				exit(EXIT_SUCCESS);

			}

		} else if (pidF[i] > 0) {

				//fluisci!

		}

	}

	close(fd_10[0]), close(fd_10[1]);
	close(fd_21[1]), close(fd_21[1]);

	while (read(fd_10, &val, sizeof(int)) != 0) {

		sum += val;

	}

	printf("Ecco la somma: %d.\n", sum);

	return EXIT_SUCCESS;
}
