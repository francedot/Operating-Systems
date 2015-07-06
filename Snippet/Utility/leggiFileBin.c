/*
 ============================================================================
 Name        : EsLetturaBin.c
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

//sintassi leggiFileBin nomeFile

int main(int argc, char** argv) {

	int fdIn, numLetto, letti;

	fdIn = open(argv[1], O_RDONLY);

	if (fdIn < 0)
		perror("err ap.");

	letti = read(fdIn, &numLetto, sizeof(int));

	while (letti > 0) {

		printf("%d ", numLetto);
		letti = read(fdIn, &numLetto, sizeof(int));

	}

	close(fdIn);
	
	printf("\n");
	return EXIT_SUCCESS;

}
