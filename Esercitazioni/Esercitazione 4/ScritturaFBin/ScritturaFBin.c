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

#define PATH "/home/francesco/Desktop/esBin"

int main(void) {

	int fd, arrayInt[10], i, numScritti;

	for (i = 0; i < 10; i++)
		arrayInt[i] = i;

	fd = creat(PATH ,00777);

	for (i = 0; i < 10; i++) {

		write(fd, &arrayInt[i], sizeof(int));

	}

	//unlink(PATH);

	close(fd);

	return EXIT_SUCCESS;

}
