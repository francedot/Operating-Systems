int isFolder(char* path)
{
	int i = 0;

	while (path[i++] != '\0');

	return path[i-2] == '/';
}