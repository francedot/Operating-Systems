int fineFile(int fd) //su stream!
{
	char c;
	if (read(fd, &c, sizeof(char)) == 0) {

		return 1;

	} else {

		lseek(fd, -(sizeof(char)), SEEK_CUR);
		return 0;
	}
}