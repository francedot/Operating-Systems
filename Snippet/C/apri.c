int apri(char* path, int modo)
{
	int fd;

	if ((fd = open(path, modo)) < 0) {

		fprintf(stderr, "P %d: errore apertura %s.\n", getpid(), path);
		exit(EXIT_FAILURE);

	}

	return fd;
}