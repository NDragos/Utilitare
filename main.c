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

// ==========================================
// TASK 1
// ==========================================

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
                        char temp[3] = {'-', argv[i][j], '\0'};
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

// ==========================================
// TASK 2
// ==========================================

void collect_dirs_recursive(char* path, int depth, Node **list, int r, int d) {
    push(list, path, depth);
    Node *current = *list;

    DIR *dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Error opening directory %s\n", path);
        return;
    }

    struct dirent *entry;
    char fullpath[PATH_MAX];

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            if (!d && depth == 0) {
                int duplicate = 0;
                for (info_files *iter = current->content; iter; iter = iter->next) {
                    if (strcmp(iter->name, entry->d_name) == 0) duplicate = 1;
                }
                if (!duplicate) {
                    snprintf(fullpath, PATH_MAX, "%s/%s", path, entry->d_name);
                    struct stat fileStat;
                    if (lstat(fullpath, &fileStat) == 0) addbegin(&current->content, entry->d_name, fileStat.st_size);
                }
            }
            continue;
        }
        
        if (strstr(entry->d_name, ":Zone.Identifier")) continue;

        snprintf(fullpath, PATH_MAX, "%s/%s", path, entry->d_name);
        struct stat fileStat;
        
        if (lstat(fullpath, &fileStat) == 0) {
            if (S_ISLNK(fileStat.st_mode)) continue;
            if (S_ISDIR(fileStat.st_mode)) {
                if (r) collect_dirs_recursive(fullpath, depth + 1, list, r, d);
                else addbegin(&current->content, entry->d_name, fileStat.st_size);
            } else if (S_ISREG(fileStat.st_mode) && !d) {
                addbegin(&current->content, entry->d_name, fileStat.st_size);
            }
        }
    }
    closedir(dir);
}

void util_ls(char* path, FILE *fout, Node **stack, int r, int d, int hr) {
    collect_dirs_recursive(path, 0, stack, r, d);
    sortdirs(stack);

    Node *temp = *stack;
    while (temp != NULL) {
        sortfiles(&(temp->content));
        temp = temp->next;
    }

    // Preserve output formatting quirk for depth 0
    if (r && *stack != NULL && (*stack)->next != NULL) {
        Node *prev = NULL, *curr = *stack, *root_node = NULL;
        while (curr != NULL) {
            if (curr->info.depth == 0) {
                root_node = curr;
                if (prev) prev->next = curr->next;
                else *stack = curr->next;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        if (root_node) {
            root_node->next = NULL;
            curr = *stack;
            while (curr && curr->next) curr = curr->next;
            if (curr) curr->next = root_node;
            else *stack = root_node;
        }
    }

    printStack(*stack, fout, hr);
    freestack(*stack);
    *stack = NULL;
}

// ==========================================
// TASK 3
// ==========================================

// Highly optimized string comparison to find the common directory
char* lcdir(const char* dir1, const char* dir2) {
    int i = 0, last_slash = -1;
    while (dir1[i] && dir2[i] && dir1[i] == dir2[i]) {
        if (dir1[i] == '/') last_slash = i;
        i++;
    }
    
    if (dir1[i] == '\0' && (dir2[i] == '/' || dir2[i] == '\0')) last_slash = i;
    else if (dir2[i] == '\0' && (dir1[i] == '/' || dir1[i] == '\0')) last_slash = i;
    
    if (last_slash == -1) return NULL;
    
    char *common = malloc(last_slash + 1);
    strncpy(common, dir1, last_slash);
    common[last_slash] = '\0';
    return common;
}

void dir_tree(Nodes *nodes) {
    if (!nodes || !nodes->dir_name) return;

    DIR *dir = opendir(nodes->dir_name);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", nodes->dir_name, entry->d_name);

        struct stat fileStat;
        if (lstat(fullpath, &fileStat) == 0 && S_ISDIR(fileStat.st_mode) && !S_ISLNK(fileStat.st_mode))
            adoptchild(nodes, fullpath);
    }
    closedir(dir);
    for (int i = 0; i < nodes->child_no; i++) dir_tree(nodes->next[i]);
}

// Recursive helper replacing the manual array allocation and reverse-iteration
void print_path_recursive(STACK *s, FILE *f, int is_target) {
    char *dir = stack_pop(s);
    if (!dir) return;
    
    if (s->top >= 0) { // Keep digging until we hit the common root
        print_path_recursive(s, f, 0);
        fprintf(f, "%s%s", dir, is_target ? "" : "/");
    }
    free(dir);
}


// ==========================================
// TASK 4
// ==========================================

void shortest_edit_script(FILE *fout, FILE *ref_file, FILE *compare_file) {
    char *ref_line = NULL, *compare_line = NULL;
    size_t ref_length = 0, compare_length = 0;
    ssize_t retention_ref_file = getline(&ref_line, &ref_length, ref_file);
    ssize_t retention_compare_file = getline(&compare_line, &compare_length, compare_file);

    while (retention_ref_file != -1 || retention_compare_file != -1) {
        const char *ref_str = (retention_ref_file != -1) ? ref_line : "";
        const char *comp_str = (retention_compare_file != -1) ? compare_line : "";

        dijkstra(ref_str, comp_str, fout);

        if (retention_ref_file != -1) retention_ref_file = getline(&ref_line, &ref_length, ref_file);
        if (retention_compare_file != -1) retention_compare_file = getline(&compare_line, &compare_length, compare_file);
    }
    free(ref_line);
    free(compare_line);
}

// ==========================================
// MAIN
// ==========================================

int main(int argc, char **argv) {

    if (strcmp(argv[1], "task1") == 0) {
        FILE *fin, *fout;
        ARG *argument = NULL, *option = NULL, *flag = NULL;
        char infile[256] = {0}, outfile[256] = {0};

        if (strncmp(argv[argc - 2], "--file=", 7) == 0) strcpy(infile, argv[argc - 2] + 7);
        if (strncmp(argv[argc - 1], "--out=", 6) == 0) strcpy(outfile, argv[argc - 1] + 6);

        fin = fopen(infile, "r");
        fout = fopen(outfile, "w");
        
        // Fail-safe to avoid 472-byte FILE leak if opening input fails
        if (!fin || !fout) {
            if (fin) fclose(fin);
            if (fout) fclose(fout);
            exit(EXIT_FAILURE);
        }

        sort_arg(argc - 2, argv + 2, &argument, &option, &flag);

        char line[100];
        while (fgets(line, sizeof(line), fin)) {
            line[strcspn(line, "\n")] = '\0';
            if (line[0] == 'a') {
                fprintf(fout, "%s ", line);
                find_arg(&argument, fout);
            } else if (strstr(line, " f")) {
                fprintf(fout, "%s ", line);
                find_f_otp(&flag, line, fout);
            } else if (strstr(line, " o")) {
                fprintf(fout, "%s ", line);
                find_f_otp(&option, line, fout);
            }
        }
        free_list(argument); free_list(option); free_list(flag);
        fclose(fin); fclose(fout);

    } else if (strcmp(argv[1], "task2") == 0) {
        Node* stack = NULL;
        int recursive = 0, human_readable = 0, dir_only = 0, opt;
        char *out_path = NULL;

        static struct option long_options[] = {
            {"recursive", no_argument, 0, 'r'},
            {"human-readable", no_argument, 0, 'h'},
            {"directory", no_argument, 0, 'd'},
            {"out", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        while((opt = getopt_long(argc, argv, "rhdo:", long_options, NULL)) != -1) {
            switch(opt) {
                case 'r': recursive = 1; break;
                case 'h': human_readable = 1; break;
                case 'd': dir_only = 1; break;
                case 'o': out_path = optarg; break;
                default: exit(EXIT_FAILURE);
            }
        }
        
        FILE *fout_2 = fopen(out_path, "wt");
        if(fout_2) {
            util_ls(argv[optind + 1], fout_2, &stack, recursive, dir_only, human_readable);
            fclose(fout_2);
        }

    } else if (strcmp(argv[1], "task3") == 0) {
        char *ref_file = argv[argc - 1];
        if (strncmp(ref_file, "--out=", 6) == 0) ref_file += 6;
        else if (strstr(ref_file, "ref")) ref_file = strstr(ref_file, "ref"); 

        FILE *f = fopen(ref_file, "wt");
        char *common = lcdir(argv[2], argv[3]);
        Nodes *dirs = NULL;
        
        if (f && common) {
            createroot(&dirs, common);
            free(common);
            dir_tree(dirs);

            STACK *p1 = dfs_stack(dirs, argv[2]);
            STACK *p2 = dfs_stack(dirs, argv[3]);
            
            if (p1 && p2) {
                int steps1 = p1->top + 1;
                for (int i = 0; i < steps1 - 1; i++) {
                    free(stack_pop(p1));
                    fprintf(f, "../");
                }
                free(stack_pop(p1));
                stack_delete(p1);

                // Use the new recursive printing function
                print_path_recursive(p2, f, 1);
                stack_delete(p2);
            } else {
                // Properly free missing/invalid paths
                if (p1) stack_delete(p1);
                if (p2) stack_delete(p2);
            }
            free_tree(dirs);
            fclose(f);
        }

    } else if(strcmp(argv[1], "task4") == 0) {
        FILE *ref_file = fopen(argv[2], "rt");
        FILE *compare_file = fopen(argv[3], "rt");
        
        char *out_filename = argv[argc - 1];
        if (strncmp(out_filename, "--out=", 6) == 0) out_filename += 6;
        FILE *fout = fopen(out_filename, "wt");

        if (ref_file && compare_file && fout) {
            shortest_edit_script(fout, ref_file, compare_file);
        }
        
        // Ensures gracefully cleaning up any partially opened files if the inputs are missing
        if (ref_file) fclose(ref_file);
        if (compare_file) fclose(compare_file);
        if (fout) fclose(fout);
    }
    
    return 0;
}