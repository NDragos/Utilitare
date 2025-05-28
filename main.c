
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>
#include<dirent.h>
#include<getopt.h>
#include<ctype.h>
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"

//task 1

void sort_arg(int argc, char *argv[], ARG **argument, ARG **option, ARG **flag) {
    for (int i = 0; i < argc; i++) {
        if (is_short(argv[i]) || is_long(argv[i])) {
            char *eq = strchr(argv[i], '=');
            if (eq) {
                opt(argv[i], eq + 1, option);
            } else if (i + 1 < argc && (isdigit(argv[i + 1][0]) ||
                       (argv[i + 1][0] == '-' && isdigit(argv[i + 1][1])))) {
                opt(argv[i], argv[i + 1], option);
                i++;
                       } else {
                           if (argv[i][0] == '-' && argv[i][1] != '-' && strlen(argv[i]) > 2) {
                               for (int j = 1; j < strlen(argv[i]); j++) {
                                   char temp[3];
                                   temp[0] = '-';
                                   temp[1] = argv[i][j];
                                   temp[2] = '\0';
                                   add_flag(flag, temp);
                               }
                           } else {
                               add_flag(flag, argv[i]);
                           }
                       }
        } else if (!atoi(argv[i]) || (argv[i][0] != '0' && !isdigit(argv[i][0]))) {
            add_arg(argument, argv[i]);
        }
    }
}


//task 2

void util_ls(char* path, FILE *fout, Node **stack, int r, int d, int hr) {

if (r) {
        // Recursive mode.
        push(stack, path, 0);
        Node *root = pop_node(stack);
        DIR *dir = opendir(root->info.name);
        if (dir == NULL) {
            fprintf(stderr, "Error opening directory %s (util_ls function)\n", root->info.name);
            free(root->info.name);
            exit(EXIT_FAILURE);
        }
        struct dirent *entry;
        char fullpath[PATH_MAX];
        // Process root directory (depth 0).
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                if (!d) {
                    int duplicate = 0;
                    info_files *iter = root->content;
                    while (iter) {
                        if (strcmp(iter->name, entry->d_name) == 0) {
                            duplicate = 1;
                            break;
                        }
                        iter = iter->next;
                    }
                    if (!duplicate) {
                        snprintf(fullpath, PATH_MAX, "%s/%s", root->info.name, entry->d_name);
                        struct stat fileStat;
                        if (lstat(fullpath, &fileStat) == 0)
                            addbegin(&root->content, entry->d_name, fileStat.st_size);
                        else
                            fprintf(stderr, "lstat failed for %s\n", fullpath);
                    }
                }
                continue;
            }
            if (strstr(entry->d_name, ":Zone.Identifier"))
                continue;
            snprintf(fullpath, PATH_MAX, "%s/%s", root->info.name, entry->d_name);
            struct stat fileStat;
            if (lstat(fullpath, &fileStat) == 0) {
                if (S_ISLNK(fileStat.st_mode))
                    continue;
                if (S_ISREG(fileStat.st_mode) && !d)
                    addbegin(&root->content, entry->d_name, fileStat.st_size);
                else if (S_ISDIR(fileStat.st_mode))
                    push(stack, fullpath, root->info.depth + 1);
            } else {
                fprintf(stderr, "lstat failed for %s\n", fullpath);
            }
        }
        closedir(dir);
        Node *processed = NULL;

        while (*stack != NULL) {
            Node *current = pop_node(stack);
            DIR *dsub = opendir(current->info.name);
            if (dsub == NULL) {
                fprintf(stderr, "Error opening directory %s (util_ls function)\n", current->info.name);
                free(current->info.name);
                exit(EXIT_FAILURE);
            }
            while ((entry = readdir(dsub))) {
                // Skip . and .. in subdirectories so they print only once.
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                if (strstr(entry->d_name, ":Zone.Identifier"))
                    continue;
                snprintf(fullpath, PATH_MAX, "%s/%s", current->info.name, entry->d_name);
                struct stat fileStat;
                if (lstat(fullpath, &fileStat) == 0) {
                    if (S_ISLNK(fileStat.st_mode))
                        continue;
                    if (S_ISREG(fileStat.st_mode) && !d)
                        addbegin(&current->content, entry->d_name, fileStat.st_size);
                    else if (S_ISDIR(fileStat.st_mode))
                        push(stack, fullpath, current->info.depth + 1);
                } else {
                    fprintf(stderr, "lstat failed for %s\n", fullpath);
                }
            }
            closedir(dsub);
            current->next = processed;
            processed = current;
        }
        // Sort directories and files.
        sortdirs(&processed);
        if (processed != NULL)
            sortfiles(&processed->content);
        if (root != NULL)
            sortfiles(&root->content);
        // Append the root so its file list is printed at the end.
        if (processed == NULL)
            processed = root;
        else {
            Node *temp = processed;
            while (temp->next)
                temp = temp->next;
            temp->next = root;
        }
        printStack(processed, fout, hr);
        freestack(processed);
    }
    else {
        // Non‑recursive mode.
        push(stack, path, 0);
        Node *processed = NULL;
        Node *current = pop_node(stack);
        DIR *dir = opendir(current->info.name);
        if (dir == NULL) {
            fprintf(stderr, "Error opening directory %s (util_ls function)\n", current->info.name);
            free(current->info.name);
            exit(EXIT_FAILURE);
        }
        struct dirent *entry;
        char fullpath[PATH_MAX];
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                if (!d) {
                    int duplicate = 0;
                    info_files *iter = current->content;
                    while (iter) {
                        if (strcmp(iter->name, entry->d_name) == 0) {
                            duplicate = 1;
                            break;
                        }
                        iter = iter->next;
                    }
                    if (!duplicate) {
                        snprintf(fullpath, PATH_MAX, "%s/%s", current->info.name, entry->d_name);
                        struct stat fileStat;
                        if (lstat(fullpath, &fileStat) == 0)
                            addbegin(&current->content, entry->d_name, fileStat.st_size);
                        else
                            fprintf(stderr, "lstat failed for %s\n", fullpath);
                    }
                }
                continue;
            }
            if (strstr(entry->d_name, ":Zone.Identifier"))
                continue;
            snprintf(fullpath, PATH_MAX, "%s/%s", current->info.name, entry->d_name);
            struct stat fileStat;
            if (lstat(fullpath, &fileStat) == 0) {
                if (S_ISLNK(fileStat.st_mode))
                    continue;
                if (S_ISREG(fileStat.st_mode) && !d)
                    addbegin(&current->content, entry->d_name, fileStat.st_size);
                else if (S_ISDIR(fileStat.st_mode))
                    addbegin(&current->content, entry->d_name, fileStat.st_size);
            } else {
                fprintf(stderr, "lstat failed for %s\n", fullpath);
            }
        }
        closedir(dir);
        processed = current;
        if (!r && processed != NULL)
            sortfiles(&processed->content);
        printStack(processed, fout, hr);
        freestack(processed);
    }
}

//task 3

char* lcdir(const char* dir1, const char* dir2) {
     char *dir1_cpy = strdup(dir1);
    char *dir2_cpy = strdup(dir2);
    if (!dir1_cpy || !dir2_cpy) {
        free(dir1_cpy);
        free(dir2_cpy);
        return NULL;
    }

    char **s1 = NULL, **s2 = NULL;
    int c1 = 0, c2 = 0;
    char *tok, *save;

    tok = strtok_r(dir1_cpy, "/", &save);
    while (tok) {
        char **temp = realloc(s1, (c1 + 1) * sizeof *s1);
        if (!temp) {
            free(dir1_cpy); free(dir2_cpy);
            return NULL;
        }
        s1 = temp;
        s1[c1++] = strdup(tok);
        tok = strtok_r(NULL, "/", &save);
    }

    tok = strtok_r(dir2_cpy, "/", &save);
    while (tok) {
        char **temp = realloc(s2, (c2 + 1) * sizeof *s2);
        if (!temp) {
            for (int i = 0; i < c1; i++) free(s1[i]);
            free(s1);
            free(dir1_cpy); free(dir2_cpy);
            return NULL;
        }
        s2 = temp;
        s2[c2++] = strdup(tok);
        tok = strtok_r(NULL, "/", &save);
    }

    int limit = c1 < c2 ? c1 : c2;
    int last = -1;
    for (int i = 0; i < limit; i++) {
        if (strcmp(s1[i], s2[i]) != 0)
            break;
        last = i;
    }

    if (last < 0) {
        for (int i = 0; i < c1; i++) free(s1[i]);
        for (int i = 0; i < c2; i++) free(s2[i]);
        free(s1); free(s2);
        free(dir1_cpy); free(dir2_cpy);
        return NULL;
    }

    size_t len = 1;
    for (int i = 0; i <= last; i++)
        len += strlen(s1[i]) + 1;
    char *common = malloc(len + 1);
    if (!common) {
        for (int i = 0; i < c1; i++) free(s1[i]);
        for (int i = 0; i < c2; i++) free(s2[i]);
        free(s1); free(s2);
        free(dir1_cpy); free(dir2_cpy);
        return NULL;
    }
    common[0] = '\0';
    for (int i = 0; i <= last; i++) {
        strcat(common, "/");
        strcat(common, s1[i]);
    }

    for (int i = 0; i < c1; i++) free(s1[i]);
    for (int i = 0; i < c2; i++) free(s2[i]);
    free(s1); free(s2);
    free(dir1_cpy); free(dir2_cpy);
    return common;
}


void dir_tree(Nodes *nodes) {
    if (!nodes || !nodes->dir_name)
        return;

    DIR *dir = opendir(nodes->dir_name);
    if (!dir) {
        fprintf(stderr, "Error opening directory %s: %s\n",
                nodes->dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s",
                 nodes->dir_name, entry->d_name);

        struct stat fileStat;
        if (lstat(fullpath, &fileStat) < 0) {
            fprintf(stderr, "lstat failed for %s: %s\n", fullpath, strerror(errno));
            continue;
        }
        if (S_ISLNK(fileStat.st_mode))
            continue;
        if (S_ISDIR(fileStat.st_mode))
            adoptchild(nodes, fullpath);
    }
    closedir(dir);
    for (int i = 0; i < nodes->child_no; i++)
        dir_tree(nodes->next[i]);
}

//task 4

void shortest_edit_script(FILE *fout, FILE *ref_file, FILE *compare_file) {
    char *ref_line = NULL, *compare_line = NULL;
    size_t ref_length = 0, compare_length = 0;
    ssize_t retention_ref_file, retention_compare_file;

    fprintf(stderr, "Entered shortest_edit_script function\n");

    retention_ref_file = getline(&ref_line, &ref_length, ref_file);
    retention_compare_file = getline(&compare_line, &compare_length, compare_file);

    if(retention_compare_file == -1)
        fprintf(stderr, "Error reading from compare file. Copied string: %s\n", compare_line);

    if(retention_ref_file)
        fprintf(stderr, "Error reading from reference file. Copied string: %s\n", ref_line);

    while (retention_ref_file != -1 && retention_compare_file != -1) {
        fprintf(stderr, "Reference line: %s", ref_line);
        fprintf(stderr, "Compare line: %s", compare_line);

        if (strcmp(ref_line, compare_line) != 0) {
            fprintf(stderr, "Line differs:\nReference: %sCompare: %s", ref_line, compare_line);
        }
        fprintf(stderr, "Dijkstra - wish me luck buddy!\n");
        dijkstra(ref_line, compare_line, fout);
        fprintf(stderr, "Dijkstra - done!\n\n");

        retention_ref_file = getline(&ref_line, &ref_length, ref_file);
        retention_compare_file = getline(&compare_line, &compare_length, compare_file);
    }

    free(ref_line);
    free(compare_line);
}

int main(int argc, char **argv) {

    if (strcmp(argv[1], "task1") == 0) {
        // run task 1
        fprintf(stderr, "Task 1\n");

        FILE *fin, *fout;
        ARG *argument, *option, *flag;
        argument = NULL;
        option = NULL;
        flag = NULL;

        char infile[256] = {0}, outfile[256] = {0};

        fprintf(stderr, "Input file? %s\n", argv[argc - 2]);
        fprintf(stderr, "Output file? %s\n", argv[argc - 1]);

        if (strncmp(argv[argc - 2], "--file=", 7) == 0) {
            strcpy(infile, argv[argc - 2] + 7);
            fprintf(stderr, "Input file: %s\n", infile);
        }
        if (strncmp(argv[argc - 1], "--out=", 6) == 0) {
            strcpy(outfile, argv[argc - 1] + 6);
            fprintf(stderr, "Output file: %s\n", outfile);
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

        sort_arg(argc - 2, argv + 2, &argument, &option, &flag);

        char line[100];

        while (fgets(line, sizeof(line), fin)) {

            line[strcspn(line, "\n")] = '\0';

            if (line[0] == 'a')
            {
                fprintf(fout, "%s ", line);
                fprintf(stderr, "%s argument: ", line);
                find_arg(&argument, fout);
            }
            else if (strstr(line, " f"))
            {
                fprintf(fout, "%s ", line);
                fprintf(stderr, "%s flag: ", line);
                find_f_otp(&flag, line, fout);
            }
            else if (strstr(line, " o"))
            {
                fprintf(fout, "%s ", line);
                fprintf(stderr, "%s option/flag: ", line);
                find_f_otp(&option, line, fout);
            }
        }

        free_list(argument);
        free_list(option);
        free_list(flag);
        fclose(fin);
        fclose(fout);

    } else if (strcmp(argv[1], "task2") == 0) {
        // run task 2
        fprintf(stderr, "Task 2\n");

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
        FILE *fout_2 = fopen(out_path, "wt");

        if(fout_2 == NULL)
        {
            fprintf(stderr, "Error opening file\n");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Recursive: %d\n", recursive);
        fprintf(stderr, "Human readable: %d\n", human_readable);
        fprintf(stderr, "Directory only: %d\n", dir_only);
        fprintf(stderr, "Output file: %s\n", out_path);

        util_ls(argv[optind + 1], fout_2, &stack, recursive, dir_only,human_readable);

        freestack(stack);
        fclose(fout_2);

    } else if (strcmp(argv[1], "task3") == 0) {
        // run task 3
        fprintf(stderr, "Task 3\n");

        char *ref_file = strstr(argv[argc - 1], "ref");
        if (!ref_file) {
            fprintf(stderr, "Last argument must contain \"ref\"\n");
            return EXIT_FAILURE;
        }
        FILE *f = fopen(ref_file, "wt");
        if (!f) {
            fprintf(stderr, "Error opening file %s: %s\n", ref_file, strerror(errno));
            return EXIT_FAILURE;
        }

        char *common = lcdir(argv[2], argv[3]);
        if (!common) {
            fprintf(stderr, "No common directory between %s and %s\n", argv[2], argv[3]);
            fclose(f);
            return EXIT_FAILURE;
        }

        Nodes *dirs = NULL;
        createroot(&dirs, common);
        free(common);
        if (!dirs) {
            fprintf(stderr, "Error allocating memory for root node\n");
            fclose(f);
            return EXIT_FAILURE;
        }

        dir_tree(dirs);

        STACK *p1 = dfs_stack(dirs, argv[2]);
        STACK *p2 = dfs_stack(dirs, argv[3]);
        if (!p1 || !p2) {
            fprintf(stderr, "Error: could not find one of the paths in the tree\n");
            if (p1)
                stack_delete(p1);
            if (p2)
                stack_delete(p2);
            free_tree(dirs);
            fclose(f);
            return EXIT_FAILURE;
        }

        int steps1 = p1->top + 1;
        for (int i = 0; i < steps1 - 1; i++) {
            free(stack_pop(p1));
            fprintf(f, "../");
        }

        free(stack_pop(p1));
        stack_delete(p1);

        int steps2 = p2->top + 1;
        char **down = malloc(steps2 * sizeof *down);
        for (int i = 0; i < steps2; i++) {
            down[i] = stack_pop(p2);
        }
        stack_delete(p2);

        for (int i = steps2 - 2; i >= 0; i--) {
            fprintf(f, "%s", down[i]);
            if (i > 0) fputc('/', f);
            free(down[i]);
        }
        free(down[steps2 - 1]);
        free(down);

        free_tree(dirs);
        fclose(f);


    } else if(strcmp(argv[1], "task4") == 0) {
        // run task 4
        fprintf(stderr, "Task 4\n");

        FILE *ref_file = fopen(argv[2], "rt");
        if (!ref_file) {
            fprintf(stderr, "Error opening reference file\n");
            return EXIT_FAILURE;
        }

        FILE *compare_file = fopen(argv[3], "rt");
        if (!compare_file) {
            fprintf(stderr, "Error opening compare file\n");
            fclose(ref_file);
            return EXIT_FAILURE;
        }

        FILE *fout = fopen(strstr(argv[argc - 1], "out") + 4, "wt");

        fprintf(stderr, "Reference file: %s\n", argv[2]);
        fprintf(stderr, "Compare file: %s\n", argv[3]);
        fprintf(stderr, "Output file: %s\n", strstr(argv[argc - 1], "out") + 4);

        shortest_edit_script(fout, ref_file, compare_file);

    }
    else {
        fprintf(stderr, "Invalid task: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return 0;
}