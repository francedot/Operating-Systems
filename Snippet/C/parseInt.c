int parseInt(char* parameter)
{
	int val;
	char *endptr = NULL;
	val = strtol(parameter, &endptr, 10);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
	{
	       perror("errore strtol");
	       exit(EXIT_FAILURE);
	}

	if (endptr == parameter)
	{
	       fprintf(stderr, "No digits were found\n");
	       exit(EXIT_FAILURE);
	}

	/* If we got here, strtol() successfully parsed a number */
	return val;
}

/**DIDN'T TRY STILL --> CTYPE
while (*str)
{
        if (!isdigit(*str))
           return false;
        else
           ++str;
}*/

