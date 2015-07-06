void leggiRiga(int fd, char* riga) //su stream!
{
	char c;
	int i = 0;

	while (read(fd, &c, sizeof(char)) > 0 && c != '\n') {

		riga[i++] = c;

	}

	riga[i] = '\0';
}