#ifndef STACK_H
#define STACK_H

struct info_dir {
    char *name;//calea până la subdirectorul respectiv, inclusiv numele său
    int depth;//adâncimea la care se află subdirectorul față de directorul de bază, dat
};

typedef struct info_dir Data;

typedef struct info_files {
    char *name;//calea până la fișier, inclusiv numele său
    long int dimension;//dimensiunea în bytes a fișierului
    struct info_files* next;
}info_files;

struct Elem {
    Data info;
    info_files *content;
    struct Elem* next;
};

typedef struct Elem Node;


void push(Node** top, const char* s);
Data pop(Node** top);
Data top(Node *top);
int isempty(const Node* top);
void sortdirs(Node **stack);
void printStack(const Node* top, FILE* f, int hr);
void freestack(Node* top);
Node* findir(Node* stack, char* path);

void addbegin(info_files **head, const char* s, long int size);
void sortfiles(info_files **head);
void printlist(const info_files *head, FILE* f, int hr);
void freelist(info_files *head);

#endif //STACK_H
