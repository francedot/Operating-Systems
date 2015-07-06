#include <stdio.h>
#define DIM 20
typedef char stringa[80];
typedef stringa strvett[DIM];

void gestoresequenza(int N, strvett vett);
int gest_stato(int S, int pid);

int main(void)
{
	int pid,ncom, stato, i;
	strvett vstr;

	printf("quanti comandi? ");
	scanf("%d", &ncom);

	for(i=0; i<ncom; i++) {

		printf("\ndammi il prossimo comando(senza argomenti)");
		scanf("%s", vstr[i]);
	}

	gestoresequenza(ncom-1,vstr);

	pid=wait(&stato);

	gest_stato(stato,pid);

	return 0;

}

void gestoresequenza(int N, strvett vett)
{
	int pid;
	pid=fork();
	if (pid==0) {/* figlio*/

		if (N==-1) {/* processo foglia */

			printf("\nfoglia %d: \n", getpid());
			exit(0);

		}
		else { /*attivazione di un nuovo comando*/

			printf("\nProcesso %d per comando %s",getpid(), vett[N]);
			gestoresequenza(N-1, vett);
			execlp(vett[N], vett[N], (char *)0);
			perror("\nexec fallita: ");
			exit(-1);

		}
	}
}

int gest_stato(int S, int pid)
{
	printf("terminato processo figlio n.%d", pid);
	if ((char)S==0)
		printf("term. volontaria con stato %d", S>>8);
	else
		printf("terminazione involontaria per segnale%d\n", (char)S);
}
