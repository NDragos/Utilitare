#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>
#include "task2.h"
#include <limits.h>

//funcțiile pentru stocarea directoarelor, de prelucrare a stivei
void push(Node **top, const char *s, int depth) {
    Node* newNode = malloc(sizeof(Node));
    if(newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->info.name = strdup(s);
    newNode->info.depth = depth;
    newNode->next = *top;
    newNode->content = NULL;
    *top = newNode;
}

Node* pop_node(Node **top) {
    if (isempty(*top))
        return NULL;
    Node *temp = *top;
    *top = (*top)->next;
    temp->next = NULL;
    return temp;
}


Data pop(Node** top) {

    if (isempty(*top)) {
        Data d = {NULL, 0};
        return d;
    }

    Node *temp = *top;
    Data aux = temp->info;
    *top = (*top)->next;
    free(temp);

    return aux;
}

Data top(Node *top){

    if (isempty(top)) {
        Data d = {NULL, 0};
        return d;
    }
    return top->info;
}

void printStack(const Node* top, FILE* f, int hr) {

    struct stat dirStat;
    while (top != NULL) {
        // Skip printing the root directory (depth 0) directory info.
        if (top->info.depth != 0) {
            for (int i = 0; i < top->info.depth; i++)
                fprintf(f, "\t");
            if (stat(top->info.name, &dirStat) == 0) {
                if (hr)
                    fprintf(f, "%ldK %s\n", dirStat.st_size / 1024, strrchr(top->info.name, '/') + 1);
                else
                    fprintf(f, "%ld %s\n", dirStat.st_size, strrchr(top->info.name, '/') + 1);
            }
        }
        // Print the file list for the directory (including for the root).
        if (top->content)
            PrintList(top->content, f, hr, top->info.depth);
        top = top->next;
    }
}

int isempty(const Node* top) {

    return top == NULL;
}

Node* findir(Node* stack, char* path) {
    Node* temp = stack;
    while(temp) {
        if(strcmp(path, temp->info.name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void sortdirs(Node **stack) {

    if (isempty(*stack) || (*stack)->next == NULL)
        return;

    int aux;

    do {

        aux = 0;
        Node **p = stack;
        while (*p && (*p)->next) {

            if (strcmp((*p)->info.name, (*p)->next->info.name) > 0) {

                Node *temp = *p;
                *p = (*p)->next;
                temp->next = (*p)->next;
                (*p)->next = temp;
                aux = 1;
            }

            p = &(*p)->next;
        }
    } while (aux);
}

void freestack(Node* top) {

    while (top) {

        Node *temp = top;
        top = top->next;

        free(temp->info.name);
       freelist(temp->content);
        free(temp);
    }
}


//funcțiile pentru stocarea fișierelor, de prelucrare a listei

void addbegin(info_files **head, const char* s, long int size) {

    info_files *newNode = malloc(sizeof(info_files));
    newNode->name = strdup(s);
    fprintf(stderr, "addbegin -> adding file: %s, size: %ld\n", newNode->name, size);
    newNode->dimension = size;
    newNode->next = *head;
    *head = newNode;
}

void sortfiles(info_files **head) {

    if (*head == NULL)
        return;

    int is_swapped;
    info_files *p;
    info_files *lastPtr = NULL;

    do {
        is_swapped = 0;
        for (p = *head; p->next != lastPtr && p->next != NULL; p = p->next) {
            if (strcmp(p->name, p->next->name) > 0) {
                char *temp_name = p->name;
                p->name = p->next->name;
                p->next->name = temp_name;

                long int temp_dim = p->dimension;
                p->dimension = p->next->dimension;
                p->next->dimension = temp_dim;
                is_swapped = 1;
            }
        }
        lastPtr = p;
    } while (is_swapped);
}

void PrintList(const info_files *head, FILE* f, int hr, int depth) {

    if(depth > 0)
        for (int i = 0; i <= depth; i++)
            fprintf(f, "\t");

    while (head != NULL) {
        char *file_name = strrchr(head->name, '/');
        if (file_name)
            file_name++;// skip the '/'
        else
            file_name = head->name;

        fprintf(f, "%ld %s\n", head->dimension, file_name);
        fprintf(stderr, "%ld %s\n", head->dimension, file_name);

        head = head->next;
    }
}


void freelist(info_files *head) {

    while (head) {
        info_files *next = head;
        head = head->next;
        free(next->name);
        free(next);
    }
}