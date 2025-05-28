#ifndef TASK3_H
#define TASK3_H

typedef char Info;

struct Elem {
    Info *dir_name;
    struct Elem** next;
    int child_no;
};

typedef struct Elem Nodes;


typedef struct stack {
    char ** elems;
    int     top;
    int     cap;
} STACK;

typedef struct node_stack {
    Nodes ** elems;
    int     top;
    int     cap;
} NSTACK;

STACK* stack_create(int initial_cap);
void stack_push(STACK *s, const char *name);
char* stack_pop(STACK *s);
int stack_is_empty(STACK *s);
void stack_delete(STACK *s);

NSTACK* nstack_create(int initial_cap);
void nstack_push(NSTACK *s, Nodes *n);
Nodes* nstack_pop(NSTACK *s);
int nstack_is_empty(NSTACK *s);
void nstack_delete(NSTACK *s);

STACK* dfs_stack(Nodes *root, const char *target_fullpath);


void createroot(Nodes** root, char* d);
void adoptchild(Nodes* root, char* d);
void free_tree(Nodes* root);

#endif //TASK3_H
