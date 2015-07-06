int contaRighe(int fd)
{
	//N.B: stream aperto il lettura!

	int numRighe = 0;
	char c;

	while (read(fd, &c, sizeof(char)) > 0) {

		if (c == '\n') {

			numRighe++;
		}

	}

	return numRighe;

}
