#ifndef LIST_H
#define LIST_H

#include <stdio.h>


struct info_arg {
    char tip;
    void* val;
    char arg_tip;
    char *id;
};

typedef struct info_arg Data;

struct arg {
    Data data;
    struct arg* next;
};

typedef struct arg ARG;

void printlist(void **head);
void add_arg(ARG **head, char* nval);
void add_opt(ARG **head, char* nval, int nr);
void add_flag(ARG **head, char* dval);
char* is_long(const char* s);
char* is_short(const char* s);
void opt(char* s1, const char*s2, ARG **opt);
void mult_arg(char *s, ARG **head);
void find_arg(ARG **head, FILE* f);
void find_f_otp(ARG **head, char* s, FILE* f);
void free_list(ARG *head);


#endif //LIST_H
