void chiudi(int fd)
{
	if (close(fd) < 0) {

		fprintf(stderr, "P %d: errore chiusura fd!\n", getpid());
		exit(EXIT_FAILURE);

	}

}