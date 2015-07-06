/*
 ============================================================================
 Name        : ScritturaFBin.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//sintassi: scriviFileBin nomeFile min max

int main(int argc, char** argv) {

	int fd, i;

	fd = creat(argv[1] ,00777);

	for (i = atoi(argv[2]); i <= atoi(argv[3]); i++) {

		write(fd, &i, sizeof(int));

	}

	close(fd);
	return EXIT_SUCCESS;

}
