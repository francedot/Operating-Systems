// Il programma deve presentare la seguente interfaccia di invocazione da linea di comando:
// esame k p input output
//
// Il significato dei parametri è il seguente:
// • k: un valore intero positivo minore o uguale a 10, che rappresenta il numero di
//   partecipanti alla lotteria.
// • p: un valore intero positivo, che rappresenta il numero di premi in palio.
// • input: è il nome assoluto di un file di input, contentente la rappresentazione binaria di
//   esattamente k numeri interi positivi a1, ..., ak,
// • output: è il nome assoluto di un file di output, sul quale dovranno essere scritti i risultati
//   della simulazione come specificato nel seguito della traccia. Se il file non è esistente
//   dovrà essere creato, altrimenti dovrà essere sovrascritto.

//Ogni partecipante alla lotteria deve essere rappresentato da un processo distinto (P1, P2, ..Pk); il
//processo iniziale P0, svolgerà invece il ruolo di coordinatore delle estrazioni.
//All’inizio della sua esecuzione, ciascun partecipante dovrà estrarre il numero di serie del suo
//biglietto della lotteria, leggendo uno degli interi contenuti nel file di input. Sfruttando i
//meccanismi che si ritengono più adeguati, si faccia in modo che nessuna coppia di partecipanti
//scelga lo stesso numero di serie, e che, dato un particolare file di input, a diverse esecuzioni del
//programma corrispondano, in generale, diversi assegnamenti di biglietti-partecipanti.
//Determinata la spartizione dei biglietti, il processo P0 dovrà effettuare, ad intervalli regolari di 3
//secondi, una delle p estrazioni scegliendo casualmente uno dei numeri di serie specificati nel
//file di input, e dovrà comunicare il numero vincente a tutti i processi partecipanti.
//Ogni partecipante dovrà autonomamente determinare se è o meno il vincitore del premio
//dell'estrazione corrente.
//Ogni processo vincitore dovrà aggiungere in coda al file di output una riga del tipo "Sono il
//partecipante con PID <x>, ed ho vinto l'estrazione <k> grazie al biglietto numero <num>.".
//Per semplicità, si faccia in modo che un biglietto scelto in un’estrazione possa vincere di nuovo
//nelle estrazioni successive.
//Alla fine delle p estrazioni, il coordinatore chiude la lotteria stampando un messaggio di fine sul
//file di output.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAX_P 10
#define MAX_DIM_STR 50

typedef char string[MAX_DIM_STR];

int label = -1;
int count = 0;

void closePipes(int pipe[][2], int side, int num);
unsigned int rand_interval(unsigned int min, unsigned int max);
void copy(char* f1, char* f2);
int estrazioneP(char* nomeFile, int numBiglietti);
int estrazioneC(char* nomeFile, int numBiglietti);
int trovaNumBiglietti(char* nomeFile);
void leggiFileBin(char* nomeFile);
int randInInt(int min, int max);
void handleStatus();
void handleChildren(int numF);
int rand_lim(int limit);

int main(int argc, char* argv[])
{

	string messFine;
	int pidP, pidF[MAX_P], i, j, numBiglietti, pipeCP[MAX_P][2], pipePC[2], temp, fdOut;
	unsigned int numPar, numPremi;

	numPar = atoi(argv[1]);
	numPremi = atoi(argv[2]);
	pidP = getpid();

	//CREAZIONE COPIA FILE DI INPUT
	copy(argv[3], "temp");

	sleep(2);

	//TROVO QUANTI SONO I BIGLIETTI
	numBiglietti = trovaNumBiglietti("temp");

	//PIPE PER COMUNICAZIONE PARTECIPANTI->COORDINATORE
	if ((pipe(pipePC)) < 0) {

		perror("Errore creazione pipe PC!\n");
		exit(EXIT_FAILURE);

	}

	//PIPES PER COMUNICAZIONE COORDINATORE-PARTECIPANTE_I_ESIMO
	for (i = 0; i < numPar; i++) {

		if ((pipe(pipeCP[i])) < 0) {

			fprintf(stderr, "Errore creazione pipe CP%d!\n", i);
			exit(EXIT_FAILURE);

		}

	}

	printf("Coordinatore %d: sono il coordinatore delle estrazioni e ho questi biglietti. Prego estraeteli pure...\n", pidP);
	leggiFileBin("temp");

	//CREAZIONE GERARCHIA A DUE LIVELLI CON SECONDO LIVELLO LUNGO numPar
	for (i = 0; i < numPar; i++)
	{

		pidF[i] = fork();

		if (pidF[i] == 0) { //Pi

			/*** INIZIO ESTRAZIONE BIGLIETTI ***/
			int biglietto, info[2];

			if (close(pipePC[0]) < 0) { //chiudo il lato di lettura!

				fprintf(stderr, "Partecipante %d: errore chiusura pipe PC lato lettura!", getpid());
				exit(EXIT_FAILURE);

			}

			biglietto = estrazioneP("temp", numBiglietti); //metto come opzione mark!
			printf("Partecipante %d: ho estratto il biglietto %d\n", getpid(), biglietto);

			//scrittura su pipe PC con l'unico scopo di far partire l'estrazione dei biglietti vincitori!
			write(pipePC[1], &label, sizeof(int));
			if (close(pipePC[1]) < 0) { //chiudo il lato di scrittura!

				fprintf(stderr, "Partecipante %d: errore chiusura pipe PC lato scrittura!", getpid());
				exit(EXIT_FAILURE);

			}

			/*** INIZIO ESTRAZIONE VINCITORI ***/
			closePipes(pipeCP, 1, numPar);

			while (read(pipeCP[i][0], info, sizeof(int)*2) > 0) {

				//"Sono il partecipante con PID <x>, ed ho vinto l'estrazione <k> grazie al biglietto numero <num>."
				if (biglietto == info[0]) {

					int fdOut, length;
					string str;

					printf("Partecipante %d: il mio biglietto %d è risultato vincente!\n", getpid(), biglietto);

					chmod(argv[4], 00777);
					if ((fdOut = open(argv[4], O_RDWR | O_APPEND)) < 0) {

						fprintf(stderr, "Partecipante %d: errore apertura file di output in modalità append!\n", getpid());
						exit(EXIT_FAILURE);

					}

					sprintf(str, "Sono il partecipante con PID %d, ed ho vinto l'estrazione %d grazie al biglietto numero %d.\n", getpid(), info[1]+1, biglietto);
					length = strlen(str);
//					printf("La lunghezza è %d.\n", length);
					write(fdOut, str, sizeof(char)*length);

					if (close(fdOut)) {

						fprintf(stderr, "Partecipante %d: errore chiusura file di output!\n", getpid());
						exit(EXIT_FAILURE);

					}

				} else if (biglietto != info[0]) {

					printf("Partecipante %d: il mio biglietto %d è risultato perdente!\n", getpid(), biglietto);

				}

			}
			if (close(pipeCP[i][0]) < 0) { //chiudo il lato di lettura!

				fprintf(stderr, "Partecipante %d: errore chiusura pipe CP%d lato lettura!\n", getpid(), i);
				exit(EXIT_FAILURE);

			}

			exit(EXIT_SUCCESS);

		} else if (pidF[i] > 0) { //P

			if (close(pipeCP[i][0]) < 0) { //chiude subito il lato di lettura!

				fprintf(stderr, "Coordinatore %d: errore chiusura pipe CP%d lato lettura!\n", getpid(), i);
				exit(EXIT_FAILURE);

			}

			//FLUISCI E CREA FIGLI!

		} else {

			perror("Errore fork!\n");

		}

	}

	if (close(pipePC[1]) < 0) {

		fprintf(stderr, "Coordinatore %d: errore chiusura pipe PC lato scrittura!\n", getpid());
		exit(EXIT_FAILURE);

	}

	while(read(pipePC[0], &temp, sizeof(int)) > 0);

	//E' ORA DI APRIRE ANCHE FILE DI OUTPUT
	if ((fdOut = creat(argv[4], 00777)) < 0) {

		fprintf(stderr, "Coordinatore %d: errore apertura file di output!\n", getpid());
		exit(EXIT_FAILURE);

	}
	//lo chiudo subito poi ci pensa il figlio ad aprirlo in modalità append!
	if (close(fdOut) < 0) {

		fprintf(stderr, "Coordinatore %d: errore chiusura file di output!\n", getpid());
		exit(EXIT_FAILURE);

	}

	/*** INIZIO ESTRAZIONE VINCITORI ***/
	printf("Coordinatore %d: tutti i partecipanti hanno concluso l'estrazione. Procedo con estrazione biglietti vincenti!\n", pidP);

	for (i = 0; i < numPremi; i++) {

		int info[2];

		sleep(3);

		printf("Coordinatore %d: inizio l' estrazione del vincitore numero %d.\n", pidP, i+1);
		info[0] = estrazioneC(argv[3], numBiglietti);
		info[1] = i;
		printf("Coordinatore %d: il biglietto vincente è il numero %d.\n", pidP, info[0]);

		for (j = 0; j < numPar; j++)
		{
			write(pipeCP[j][1], info, sizeof(int)*2);
		}

	}

	//CHIUSURA PIPES CPi

	closePipes(pipeCP, 1, numPar);

	//sostituita da sopra
//	for (i = 0; i < numPar; i++) {
//
//		//kill(pidF[i], SIGTERM);
//
//		if (close(pipeCP[i][1]) < 0) { //chiudo il lato di scrittura!
//
//			fprintf(stderr, "Coordinatore %d: errore chiusura pipe CP%d lato scrittura!\n", getpid(), i);
//			exit(EXIT_FAILURE);
//
//		}
//
//	}

	sleep(5);
	handleChildren(numPar + 1); //attendo anche il figlio andatonesene per la copy!

	if ((fdOut = open(argv[4], O_WRONLY | O_APPEND)) < 0) {

		fprintf(stderr, "Coordinatore %d: errore apertura file di output!\n", pidP);
		exit(EXIT_FAILURE);

	}

	sprintf(messFine, "Coordinatore %d: FINE.\n", pidP);
	write(fdOut, messFine, sizeof(char)*strlen(messFine));

	if (close(fdOut) < 0) {

		fprintf(stderr, "Coordinatore %d: errore chiusura file di output!\n", pidP);
		exit(EXIT_FAILURE);

	}

	return EXIT_SUCCESS;

}

void closePipes(int pipe[][2], int side, int num)
{
	int j;
	for (j = 0; j < num; j++) {

		//N.B non fd = close !!!!
		if (close(pipe[j][side]) < 0) { //chiudo il lato di scrittura!

			fprintf(stderr, "Processo %d: errore chiusura pipe %d lato scrittura!", getpid(), j);
			exit(EXIT_FAILURE);
		}
	}
	return;
}

void copy(char* f1, char* f2)
{

	if (fork() == 0) {

		execlp("cp", "cp", f1, f2, (char *)0);

	}

	return;
}

int estrazioneP(char* nomeFile, int numBiglietti)
{
	//printf("Qui estraggo\n");
	int fd, randomIndex, biglietto;

	if ((fd = open(nomeFile, O_RDWR)) < 0) {

		fprintf(stderr, "Errore apertura %s.\n", nomeFile);
		exit(-1);

	}

	do
	{
		randomIndex = randInInt(0, numBiglietti-1);
		lseek(fd, randomIndex*sizeof(int), SEEK_SET);
		read(fd, &biglietto, sizeof(int));

	} while (biglietto == -1); //per ipotesi -1 è entry già presa!

	lseek(fd, -sizeof(int), SEEK_CUR);
	write(fd, &label, sizeof(int));

	close(fd); //N.B: chiusura
	return biglietto;
}

int estrazioneC(char* nomeFile, int numBiglietti)
{
	int fd, randomIndex, bigliettoVincente;

	if ((fd = open(nomeFile, O_RDWR)) < 0) {

		fprintf(stderr, "Errore apertura %s.\n", nomeFile);
		exit(-1);

	}

	randomIndex = randInInt(0, numBiglietti-1);//-1 perchè puntatore!
	lseek(fd, randomIndex*sizeof(int), SEEK_SET);
	read(fd, &bigliettoVincente, sizeof(int));

	close(fd); //N.B: chiusura
	return bigliettoVincente;
}

int trovaNumBiglietti(char* nomeFile)
{
	int tmp, fd, num = 0;

	if ((fd = open(nomeFile, O_RDONLY)) < 0) {

		fprintf(stderr, "Errore apertura %s.\n", nomeFile);
		exit(-1);

	}

	while (read(fd, &tmp, sizeof(int)) > 0)
			num++;

	close(fd);
	return num;
}

void leggiFileBin(char* nomeFile)
{
	int fdIn, numLetto, letti;

	fdIn = open(nomeFile, O_RDONLY);

	if (fdIn < 0)
		perror("err ap.");

	letti = read(fdIn, &numLetto, sizeof(int));

	while (letti > 0) {

		printf("%d ", numLetto);
		letti = read(fdIn, &numLetto, sizeof(int));

	}

	close(fdIn);

	printf("\n");

}

int randInInt(int min, int max)
{
	int r, seed;
	seed = getpid() * time(NULL);
	srandom(seed);
	r = (random() % (max+1-min)) + min;
	return r;
}

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

unsigned int rand_interval(unsigned int min, unsigned int max)
{
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = random();
    } while (r >= limit);

    return min + (r / buckets);
}

int rand_lim(int limit)
{
	/* return a random number between 0 and limit inclusive.*/

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}
