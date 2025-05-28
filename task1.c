#include "task1.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

char* is_long(const char* s) {
    return strstr(s, "--");
}

char* is_short(const char* s) {
  return strstr(s, "-");
}

void opt(char *s1, const char* s2, ARG **opt){
  if(strstr(s1, "=")) {
    add_opt(opt, s1, atoi(strstr(s1, "=") + 1));
    return;
  }
  if(s2 && (isdigit(s2[0]) || (s2[0] == '-' && isdigit(s2[1]))))
    add_opt(opt, s1, atoi(s2));
}

void add_opt(ARG **head, char* nval, int nr){
  ARG *newNode = malloc(sizeof(ARG));
  newNode->data.val = malloc(sizeof(int));
  *(int*)newNode->data.val = nr;
  if (is_long(nval))
    newNode->data.arg_tip = 'l';
  if (is_short(nval))
    newNode->data.arg_tip = 's';
  newNode->data.id = strdup(nval);// duplicate the input string
  newNode->data.tip = 'o';
  newNode->next = *head;
  *head = newNode;
}

void add_arg(ARG **head, char* nval){
  ARG *newNode = malloc(sizeof(ARG));
  newNode->data.val = strdup(nval);// already duplicating
  newNode->data.tip = 'a';
  newNode->data.id = strdup(nval);// duplicate the input string
  newNode->next = *head;
  *head = newNode;
}

void add_flag(ARG **head, char* dval){
  ARG *newNode = malloc(sizeof(ARG));
  newNode->data.val = malloc(sizeof(int));
  *(int*)newNode->data.val = 1;// toggle flag to 1
  newNode->data.tip = 'f';
  if (is_long(dval))
    newNode->data.arg_tip = 'l';
  else if (is_short(dval))
    newNode->data.arg_tip = 's';
  newNode->data.id = strdup(dval);// duplicate the input string
  newNode->next = *head;
  *head = newNode;
}

void mult_arg(char *s, ARG **head){
  int len = strlen(s);
  for(int i = 2; i < len; i++){
	char temp[3];
	temp[0] = '-';
	temp[1] = s[i];
	temp[2] = '\0';
    switch ((*head)->data.tip){
      case 'f':
        add_flag(head, temp);
        break;

      case 'o':
        add_opt(head, temp, *(int*)(*head)->data.val);
        break;

      default:
        break;
    }
  }
}

void find_f_otp(ARG **head, char* s, FILE* f) {
  if (*head == NULL) {
    fprintf(f, " \n");
    fprintf(stderr, "!!!NULL!!!\n");
    return;
  }

  char aux[100] = {0};
  switch ((*head)->data.arg_tip) {
    case 's': {
      char* token = strtok(s, " ");
      if(token)
        strcpy(aux, token);
      else
        aux[0] = '\0';
      break;
    }
    case 'l': {
      char* p = strstr(s, "--");
      if(p) {
        char* token = strtok(p, " ");
        if(token)
          strcpy(aux, token);
        else
          aux[0] = '\0';
      } else
        aux[0] = '\0';
      break;
    }
    default:
      aux[0] = '\0';
    break;
  }

  ARG *headcopy = *head;
  if (strcmp(headcopy->data.id, aux) == 0) {
    *head = (*head)->next;
    if (headcopy->data.tip == 'o') {
      fprintf(stderr, "OPTION %d\n", *(int*)headcopy->data.val);
      fprintf(f, "%d \n", *(int*)headcopy->data.val);
    } else {
      fprintf(f, "%d \n", *(int*)headcopy->data.val);
      fprintf(stderr, "FLAG %d\n", *(int*)headcopy->data.val);
    }

    free(headcopy->data.val);
    free(headcopy->data.id);
    free(headcopy);

    return;
  }

  ARG *prev = *head;
  headcopy = headcopy->next;
  while (headcopy != NULL) {
    if (strcmp(headcopy->data.id, aux) == 0) {
      prev->next = headcopy->next;
      if (headcopy->data.tip == 'o')
        fprintf(f, "%d \n", *(int*)headcopy->data.val);

      else
        fprintf(f, "%d \n", *(int*)headcopy->data.val);

      free(headcopy->data.val);
      free(headcopy->data.id);
      free(headcopy);

      return;
    }
    prev = headcopy;
    headcopy = headcopy->next;
  }
  fprintf(f, "\n");
}

void find_arg(ARG **head, FILE* f) {
  if (*head == NULL) {
    fprintf(f, "\n");
    fprintf(stderr, "!!!NULL!!!\n");

    return;
  }

  if ((*head)->next == NULL) {
    fprintf(f, "%s \n", (char*)(*head)->data.val);
    fprintf(stderr, "%s \n", (char*)(*head)->data.val);

    free((*head)->data.val);
    free((*head)->data.id);
    free(*head);

    *head = NULL;

    return;
  }

  ARG *headcopy = *head;
  while (headcopy->next->next != NULL)
    headcopy = headcopy->next;

  fprintf(f, "%s \n", (char*)headcopy->next->data.val);
  fprintf(stderr, "%s \n", (char*)headcopy->next->data.val);

  free(headcopy->next->data.val);
  free(headcopy->next->data.id);
  free(headcopy->next);

  headcopy->next = NULL;
}

void free_list(ARG *head) {
  ARG* temp;
  while (head != NULL) {
    temp = head;
    head = head->next;

    free(temp->data.val);
    free(temp->data.id);
    free(temp);
  }
}
