#include <stdio.h>
#include <stdlib.h>

int main(int arg, char* argv[])
{
	FILE* fin, *fout;

	if((fin = (FILE*)fopen(argv[1], "rt")) == NULL)
	{
		perror("fin error");
		exit(1);
	}

	if((fout = (FILE*)fopen(argv[2], "wt")) == NULL)
	{
		perror("fout error");
		exit(2);
	}

	fprintf(fout, "This is a test. Great job and continue coding!");
	fclose(fin);
	fclose(fout);
	return 0;
}
