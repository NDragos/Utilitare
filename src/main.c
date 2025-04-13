#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<getopt.h>
#include<dirent.h>
#include "task2.h"

void util_ls(char* path, FILE *fout, Node **stack, int r, int d, int hr) {

    Node *temp = NULL;
    int fl = 0;

    //fprintf(fout, "%d %d %d\n", r, d, hr);

    //push(stack, path);
    push(&temp, path);

    while(temp != NULL) {

        Data current_data = pop(&temp);
        char *current_path = current_data.name;
        DIR *dir = opendir(current_path);



        if (dir == NULL)
        {
            fprintf(stderr, "Error opening directory %s (util_ls function)\n", path);
            free(current_path);
            exit(EXIT_FAILURE);
        }

        struct dirent *entry;

        while((entry = readdir(dir))) {

                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    if(fl == 0) {
                         struct stat dirStat;
                        if (stat(".", &dirStat) == 0) {
                           if(hr)
                                fprintf(fout ,"%.1fK %s\n", dirStat.st_size / 1024.0, ".");
                           else
                                fprintf(fout, "%ld %s\n", dirStat.st_size, ".");
                            if(stat("..", &dirStat) == 0) {
                                if(hr)
                                    fprintf(fout ,"%.1fK %s\n", dirStat.st_size / 1024.0, "..");
                                else
                                    fprintf(fout, "%ld %s\n", dirStat.st_size, "..");
                            }
                        }
                    }


                    fl ++;
                    continue;
                }


                char fullpath[PATH_MAX];
                snprintf(fullpath, PATH_MAX, "%s/%s", current_path, entry->d_name);

                struct stat fileStat;
                if(lstat(fullpath, &fileStat) == 0) {

                    if (S_ISLNK(fileStat.st_mode))
                        continue;

                    if(S_ISREG(fileStat.st_mode) && !d)
                        {
                            Node* aux = findir(*stack, current_path);
                            if(aux) {
                                //fprintf(fout, "%s\n", current_path);
                                addbegin(&aux->content, entry->d_name, fileStat.st_size);
                            }

                        }else
                            if(S_ISDIR(fileStat.st_mode))
                            {
                                push(stack, fullpath);
                                push(&temp, fullpath);
                            }
                }
                else
                    fprintf(stderr,"lstat failed for %s\n", current_path);
            }
        closedir(dir);
        free(current_path);
        if(!d && !r)
            break;
    }

    sortdirs(stack);
    sortfiles(&(*stack)->content);

    printStack(*stack, fout, hr);
    //printlist((*stack)->content, fout, hr);

    freestack(temp);
}

int main(int argc, char **argv) {
    Node* stack = NULL;

    int recursive = 0, human_readable = 0, dir_only = 0;

    int opt;
    static struct option long_options[] = {
        {"recursive", no_argument, 0, 'r'},
        {"human-readable", no_argument, 0, 'h'},
        {"directory", no_argument, 0, 'd'},
        {"out", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    char *out_path = NULL;

    while((opt = getopt_long(argc, argv, "rhdo:", long_options, NULL)) != -1) {
        switch(opt)
        {

            case 'r':
                recursive = 1;
            break;
            case 'h':
                human_readable = 1;
            break;
            case 'd':
                dir_only = 1;
            break;
            case 'o':
                out_path = optarg;
                break;
            default:
                exit(EXIT_FAILURE);

        }
    }
    FILE *fout = fopen(out_path, "wt");

        if(fout == NULL)
        {
            fprintf(stderr, "Error opening file\n");
            exit(EXIT_FAILURE);
        }

    util_ls(argv[optind + 1], fout, &stack, recursive, dir_only,human_readable);

    freestack(stack);
    fclose(fout);
    return 0;
}
