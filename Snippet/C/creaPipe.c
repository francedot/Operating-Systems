//N.B: lato di scrittura = 1
//     lato di lettura = 0

void creaPipe(int _pipe[])
{
	if (pipe(_pipe) < 0) {

		fprintf(stderr, "P0 %d: errore creazione pipe!\n", getpid());
		exit(EXIT_FAILURE);

	}
}