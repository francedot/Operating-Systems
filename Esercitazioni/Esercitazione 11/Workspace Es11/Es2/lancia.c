//Si realizzi un programma C di sistema che, utilizzando
//le system call Unix, preveda la seguente sintassi:
//lancia fileout timeout tipo_op
//Dove:
//-fileout è il nome di un file scrivibile
//-timeout è un intero positivo.
//-tipo_op è un carattere che può assumere soltanto 2
//valori: ‘d’ o ‘a’.
//Il processo iniziale P0 dovrà creare un processo figlio
//P1 il cui compito sarà quello di eseguire l’applicazione
//java realizzata come soluzione dell’esercizio 1, in
//modo tale che il suo output venga ridirezionato nel file
//fileout.

//Il processo P0, dopo timeout secondi dovrà terminare
//forzatamente l’esecuzione di P1.
//Una volta terminato P1, P0 dovrà stampare sullo standard
//output il contenuto del file fileout, filtrato secondo il
//seguente criterio:
//•  Se tipo_op = ‘a’, verranno stampate le linee
//contenenti la stringa “atterraggio”.
//•  Se tipo_op = ‘d’, verranno stampate le linee
//contenenti la stringa “decollo”.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_DIM_STR 100

typedef char string[MAX_DIM_STR];

int pidP1;

int isFolder(char* path)
{
	int i = 0;

	while (path[i++] != '\0');

	return path[i-2] == '/';
}

void notifyAlarm(int signo)
{
	kill(pidP1, SIGTERM);
}

void handleStatus()
{
	int pidAtteso, status;
	pidAtteso = wait(&status);

	if (pidAtteso < 0) {

		perror("Errore Attesa Figlio!");

	} else if (WIFEXITED(status)) {

		printf("P %d: Processo F %d terminato volontariamente con stato %d.\n", getpid(), pidAtteso, WEXITSTATUS(status));

	}  else if (WIFSIGNALED(status)) {

		printf("P %d: Processo F %d terminato involontariamente causa segnale %d.\n", getpid(), pidAtteso, WTERMSIG(status));

	}
}

void handleChildren(int numF)
{
	int i;

	for (i = 0; i < numF; i++)
			handleStatus();
}

int fineFile(int fd) //su stream!
{
	char c;
	if (read(fd, &c, sizeof(char)) == 0) {

		return 1;

	} else {

		lseek(fd, -(sizeof(char)), SEEK_CUR);
		return 0;
	}
}

void leggiRiga(int fd, char* riga) //su stream!
{
	char c;
	int i = 0;

	while (read(fd, &c, sizeof(char)) > 0 && c != '\n') {

		riga[i++] = c;

	}

	riga[i] = '\0';
}

int apri(char* path, int modo)
{
	int fd;

	if ((fd = open(path, modo)) < 0) {

		fprintf(stderr, "P %d: errore apertura %s.\n", getpid(), path);
		exit(EXIT_FAILURE);

	}

	return fd;
}

void chiudi(int fd)
{
	if (close(fd) < 0) {

		fprintf(stderr, "P %d: errore chiusura fd!\n", getpid());
		exit(EXIT_FAILURE);

	}

}

int main(int argc, char** argv)
{

	int pidP0, i = 0, fdOut;
	string riga, toSeek;

	if (argc != 4) {

		perror("Errore numero comandi.\n");
		exit(EXIT_FAILURE);

	}

	if (isFolder(argv[1])) {

		perror("fileOut deve essere un file!\n");
		exit(EXIT_FAILURE);

	}

	if (argv[3][0] != 'd' && argv[3][0] != 'a') {

		perror("Parametro 4 deve essere d o a!\n");
		exit(EXIT_FAILURE);

	}

	pidP0 = getpid();

	signal(SIGALRM, &notifyAlarm);
	alarm(atoi(argv[2]));

	printf("P0: %d.\n", pidP0);

	if ((pidP1 = fork()) == 0) { //P1

		int pidP2;

		printf("P1: %d.\n", getpid());
		//compilo ed eseguo file java!
		//compilazione affidata a P2 figlio di P1

		if ((pidP2 = fork()) == 0) { //P2 --> compilazione

			printf("P2: %d.\n", getpid());
			//execlp("pwd", "pwd", (char *)0);
			//execlp("pwd", "pwd", (char *)0);

			chdir("..");
			chdir("Aerei/src/"); //prova a unirle

			execlp("javac", "javac", "Aereo.java", "Aereoporto.java", "Mezzo.java", "Pista.java", (char *)0);
			perror("errore exec\n");
			exit(EXIT_FAILURE);

		} else if (pidP2 > 0) { //P1 esecuzione

			handleChildren(1);
			//compilazione terminata

			//esecuzione AereoPorto
			//execlp("pwd", "pwd", (char *)0);
			//chdir("..");
			if ((fdOut = open(argv[1], O_WRONLY)) < 0) {

				perror("Errore apertura fdOut.\n");
				exit(EXIT_FAILURE);

			}

			close(1);
			if (dup(fdOut) < 0) {

				perror("Errore dup.\n");
				exit(EXIT_FAILURE);

			}

			chdir("..");
			chdir("Aerei/src/"); //prova a unirle
			//execlp("pwd", "pwd", (char *)0);

			//chi chiude il file?
			execlp("java", "java", "Aereoporto", (char *)0);
			perror("Errore exec.\n");
			exit(EXIT_FAILURE);

		} else {

			perror("Errore fork P1.\n");
			exit(EXIT_FAILURE);

		}

	} else if (pidP1 > 0) { //P0s

		//fluisci

	} else {

		perror("Errore fork P0.\n");
		exit(EXIT_FAILURE);

	}

	//P0

	//terminazione dopo  timeout secondi

	handleChildren(1);

	printf("Procedo con il filtraggio delle righe.\n");

	fdOut = apri(argv[1], O_RDONLY);

	strcpy(toSeek, ((argv[3][0] == 'd') ? "decollo" : "atterraggio"));

	while (!fineFile(fdOut)) {

		leggiRiga(fdOut, riga);

		if (strstr(riga, toSeek) != NULL) {

			printf("%s\n", riga);

		}

	}

	chiudi(fdOut);

	return EXIT_SUCCESS;

}
