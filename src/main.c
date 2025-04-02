#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void sort_arg(int argc, char *argv[], ARG **argument, ARG **option, ARG **flag) {
    for (int i = 0; i < argc; i++) {
        if(is_short(argv[i])) {
            if(atoi(argv[i]) || (i + 1 < argc && atoi(argv[i + 1]))) {
                opt(argv[i], argv[i + 1], option);
                if(strlen((*option)->data.id) > 2) {
			char *dup = strdup((*option)->data.id);
			(*option)->data.id = dup;
                    mult_arg((*option)->data.id, option);
                    (*option)->data.id[2] = '\0';
                }

                if(atoi(argv[i + 1])) i++;
            }
            else {
                add_flag(flag, argv[i]);
                if(strlen((*flag)->data.id) > 2) {
			char *dup = strdup((*flag)->data.id);
			(*flag)->data.id = dup;
                     mult_arg((*flag)->data.id, flag);
                    (*flag)->data.id[2] = '\0';
                }
            }
        }
        else add_arg(argument, argv[i]);
    }

}

int main(int argc, char **argv){
    FILE *fin, *fout;
    ARG *argument, *option, *flag;
	argument = NULL;
	option = NULL;
	flag = NULL;
    sort_arg(argc - 2, argv + 2, &argument, &option, &flag);

    char infile[256] = {0}, outfile[256] = {0};
        if (strncmp(argv[argc - 2], "--file=", 7) == 0) {
		const char* posin = strstr(argv[argc - 2] + 7, "teste");
	if(posin)
		strcpy(infile, posin);
	else
            strcpy(infile, argv[argc - 2] + 7);
        }
	if (strncmp(argv[argc - 1], "--out=", 6) == 0) {
		const char* posout = strstr(argv[argc - 1] + 6, "ref");
	if(posout)
		strcpy(outfile, posout);
	else
            strcpy(outfile, argv[argc - 1] + 6);
        }

    fin = fopen(infile, "r");
    if (!fin) {
        fprintf(stderr, "Error opening file for reading: %s", infile);
        exit(1);
    }

    fout = fopen(outfile, "w");
    if (!fout) {
        perror("Error opening file for writing");
        fclose(fin);
        exit(2);
    }

    char line[100];

    while (fgets(line, sizeof(line), fin)) {

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == 'a')
            {
            fprintf(fout, "%s ", line);
            find_arg(&argument, fout);
            }
        else if (strstr(line, " f"))
            {
            fprintf(fout, "%s ", line);
            find_f_otp(&flag, line, fout);
            }
        else if (strstr(line, " o"))
            {
            fprintf(fout, "%s ", line);
            find_f_otp(&option, line, fout);
            }
    }

    free_list(argument);
    free_list(option);
    free_list(flag);
    fclose(fin);
    fclose(fout);
    return 0;
}
