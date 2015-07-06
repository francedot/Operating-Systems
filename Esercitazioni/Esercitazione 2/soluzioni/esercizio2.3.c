/*
Sintassi di invocazione: 
eseguiComandi K COM1 COM2 ... COMN 
Significato degli argomenti: 
• eseguiComandi è il nome del file eseguibile associato al 
programma. 
• COM1, COM2, ... ,COMN sono N stringhe che rappresentano il 
nome di un file (
• K e` un valore intero positivo (minore di N)‏32 
Specifiche 
Il processo iniziale (P0) deve mettere in esecuzione gli N 
comandi passati come argomenti, secondo la seguente logica: 
• i primi K comandi passato come argomenti dovranno essere 
eseguiti in parallelo da altrettanti figli di P0 
• al termine dei primi K processi, i restanti N-K comandi dovranno 
essere eseguiti in sequenza da altrettanti figli e/o nipoti di P0 
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char* argv[]){

	//Controllo sul numero dei parametri
	if(argc <= 2){
		printf("Numero parametri non sufficiente\n");
		printf("Uso: ./%s K COM1 COM2 ... COMN\n", argv[0]);
		exit(1);
	}

	//Controllo che il primo parametro sia un numero
	if(!isdigit(argv[1][0])){
		printf("Il secondo parametro non è un numero\n");
		printf("Uso: ./%s K COM1 COM2 ... COMN\n", argv[0]);
		exit(1);
	}
	
	int num_comandi = argc - 2;
	int k = atoi (argv[1]);
	int i, pid, status;
	
	//Verifico che K sia minore del numero di comandi
	if(k > num_comandi){
		printf("Attenzione: K deve essere minore del numero di comandi\n");
		exit(1);
	}
	
	for(i=0; i < k; i++){
		pid=fork();
		if(pid==0){
			//Codice figlio
			printf("\nFIGLIO %d: Esecuzione in parallelo del comando %s\n", getpid(), argv[i+2]);
			execlp(argv[i+2], argv[i+2], (char *) 0);
			//L'exit(1) viene inserita nel caso il cui il programma specificato non venga trovato
			exit(1);
		}else if (pid < 0){
			printf("Errore nella fork!\n");
			exit(1);
		}
	}
	
	//Il padre attende i figli concorrenti appena creati
	for(i=0; i < k; i++){
		pid=wait(&status);
		printf("PADRE: terminazione del figlio %d con stato %d\n", pid, WEXITSTATUS(status));
	}
	
	/*
	* Il padre crea un figlio e attende la sua terminazione prima di creare il successivo.
	* Ho quindi un'esecuzione sequenziale
	*/
	for(; i < num_comandi; i++){
		pid=fork();
		if(pid==0){
			//Codice figlio
			printf("\nFIGLIO %d: Esecuzione sequenziale comando %s\n", getpid(), argv[i+2]);
			execlp(argv[i+2], argv[i+2], (char *) 0);
			exit(1);
		} else if (pid > 0) {
			pid=wait(&status);
			printf("PADRE: terminazione del figlio %d con stato %d\n", pid, WEXITSTATUS(status));
		} else {
			printf("Errore nella fork!\n");
			exit(1);
		}
	}
	
	exit(0);
}
