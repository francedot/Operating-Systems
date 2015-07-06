/*
 ============================================================================
 Name        : ProvaExec.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>


int main(void) {

	//execlp("/home/francesco/Desktop/ls", "/home/francesco/Desktop/ls", (char *)0);
	//perror("dd");

	int pidF, pidN, statusF;

	pidF = fork();

	if(pidF == 0) {

		execlp("gcc", "gcc", "../Assets/Sorgenti/file1.c", "-o", "../Assets/Eseguibili/file1", (char *)0);
		perror("dd");
	} else {

		wait(&statusF);	//sospensione se arriva prima il padre!

		if(WIFEXITED(statusF)) {

			execl("../Assets/Eseguibili/file1", "../Assets/Eseguibili/file1", (char *)0);

		}

	}

	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
