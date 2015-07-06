
//04 read access
//02 write access
//01 execute access
//00 existence

#define READ_ACCESS 04
#define WRITE_ACCESS 02
#define EXECUTE_ACCESS 01
#define EXISTENCE 00

void accesso(char** path, int modo)
{
	if (access(path, modo) < 0) {

		perror("Errore: file non esistente o non si ha il diritto scelto su quel file.\n");
		exit(EXIT_FAILURE);

	}

	return;
}