
//creazione gerarchia 2 livelli

pidP0 = getpid();
printf("P0 %d: sono P0 %d.\n", pidP0, pidP0);

pidP1 = fork();
if (pidP1 > 0) { //P0

	printf("P0 %d: ho creato il figlio P1 %d.\n", pidP0, pidP1);
	exit(EXIT_SUCCESS);

} else if (pidP1 == 0) { //P1

	printf("P1 %d: sono stato creato da P0 %d.\n", getpid(), pidP0);

	//creo 1 altro livello
	pidP1 = getpid(); //sovrascrivo con quello che mi interessa
	pidP2 = fork();

	if (pidP2 > 0) { //P1

		printf("P1 %d: ho creato il figlio P2 %d.\n", pidP1, pidP2);
		exit(EXIT_SUCCESS);

	} else if (pidP2 == 0) { //P2

		printf("P2 %d: sono stato creato da P1 %d.\n", getpid(), pidP1);
		exit(EXIT_SUCCESS);

	} else if (pidP2 <  0) { //errore

		perror("Errore fork2.");
		exit(EXIT_FAILURE);

	}

	exit(EXIT_SUCCESS);

} else if (pidP1 <  0) { //errore

	perror("Errore fork1.");
	exit(EXIT_FAILURE);

}