#include <stdlib.h>
#include <stdio.h>

//SINTASSI: randInInt min max
//min e max compresi

int main(int argc, char** argv) {

	int r, seed, min, max;

	min = atoi(argv[1]);
	max = atoi(argv[2]);
	
	seed = getpid() * time(NULL);
	srandom(seed);
	r = (random() % (max+1-min)) + min;

	printf("r=%d\n", r);

	return EXIT_SUCCESS;

}