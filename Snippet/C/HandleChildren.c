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
