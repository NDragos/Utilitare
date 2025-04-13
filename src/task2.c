#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>
#include "task2.h"
#include <limits.h>

//funcțiile pentru stocarea directoarelor, de prelucrare a stivei
void push(Node** top, const char *s) { //*s este subdirectorul

    Node* newNode = malloc(sizeof(Node));
    if(newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->info.name = strdup(s);
    newNode->info.depth = 0;
    newNode->next = *top;
    newNode->content = NULL;
    *top = newNode;

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
        if (stat(top->info.name, &dirStat) == 0)
        {
            if(hr)
                fprintf(f ,"%.1fK %s\n", dirStat.st_size / 1024.0, strrchr(top->info.name, '/') + 1);
            else
                fprintf(f, "%ld %s\n", dirStat.st_size, strrchr(top->info.name, '/') + 1);
        }

        if(top->content)
            printlist(top->content, f, hr);

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
    newNode->dimension = size;
    newNode->next = *head;
    *head = newNode;
}

void sortfiles(info_files **head) {

    if (*head == NULL) return;

    int aux;
    info_files *p;
    info_files *lp = NULL;

    do {
        aux = 0;
        p = *head;

        while (p->next != lp) {
            if (strcmp(p->name, p->next->name) > 0) {

                char *temp_name = p->name;
                p->name = p->next->name;
                p->next->name = temp_name;

                long int temp_dim = p->dimension;
                p->dimension = p->next->dimension;
                p->next->dimension = temp_dim;

                aux = 1;
            }
            p = p->next;
        }
        lp = p;
    } while (aux);
}

void printlist(const info_files *head, FILE* f, int hr) {

    while (head != NULL) {
            if(hr)
                fprintf(f ,"%.1fK %s\n", head->dimension / 1024.0, strrchr(head->name, '/') + 1);
            else
                fprintf(f, "%ld %s\n", head->dimension, strrchr(head->name, '/') + 1);
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