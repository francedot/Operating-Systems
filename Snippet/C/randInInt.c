#include <time.h> //N.B

int randInInt(int min, int max)
{
	int r, seed;
	seed = getpid() * time(NULL);
	srandom(seed);
	r = (random() % (max+1-min)) + min;
	return r;
}