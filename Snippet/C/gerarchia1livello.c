
//creazione gerarchia 1 livello

pidP0 = getpid();
pidP1 = fork();

if (pidP1 > 0) { //P0

	printf("P0 %d: ho creato il figlio P1 %d.\n", pidP0, pidP1);
	exit(EXIT_SUCCESS);

} else if (pidP1 == 0) { //P1

	printf("P1 %d: sono stato creato da P0 %d.\n", getpid(), pidP0);
	exit(EXIT_SUCCESS);

} else if (pidP1 <  0) { //errore

	perror("Errore fork1.");
	exit(EXIT_FAILURE);

}