#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* is_long(const char* s) {
    return strstr(s, "--");
}

char* is_short(const char* s) {
  return strstr(s, "-");
}

void opt( char *s1, const char* s2, ARG **opt){
    if(strstr(s1, "="))
      {
        add_opt(opt, s1, atoi(strstr(s1, "=") + 1));
        return;
      }
    if(atoi(s2))
        add_opt(opt, s1, atoi(s2));
}

void add_opt(ARG **head, char* nval, int nr){
  ARG *newNode = (ARG *)malloc(sizeof(ARG));
  newNode->data.val = malloc(sizeof(int));
  *(int*)newNode->data.val = nr;//valoare numerica
  if(is_long(nval))
    newNode->data.arg_tip = 'l';
   else if(is_short(nval))
    newNode->data.arg_tip = 's';
  newNode->data.id = nval;
  newNode->data.tip = 'o';
	newNode->next = *head;
	*head = newNode;
}

void add_arg(ARG **head, char* nval){
	ARG *newNode = malloc(sizeof(ARG));
	newNode->data.val = strdup(nval);//valoare argument
  newNode->data.tip = 'a';
	newNode->next = *head;
	*head = newNode;
}

void add_flag(ARG **head, char* dval){
	ARG *newNode = (ARG*)malloc(sizeof(ARG));
    newNode->data.val = malloc(sizeof(int));
    *(int*)newNode->data.val = 1;//toggle
    newNode->data.tip = 'f';
  if(is_long(dval))
    newNode->data.arg_tip = 'l';
  else if(is_short(dval))
    newNode->data.arg_tip = 's';
    newNode->data.id = dval;
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
    fprintf(f, "\n");
    return;
  }

  char aux[100];
  switch ((*head)->data.arg_tip) {
    case 's':
      char* token = strtok(s, " ");
	if(token)
		strcpy(aux, token);
	else
		aux[0] = '\0';
      break;
    case 'l':
      char* p = strstr(s, "--");
	if(p){
		char* token = strtok(p, " ");
		if(token)
			strcpy(aux, token);
		else aux[0] = '\0';
}	else aux[0] = '\0';
      break;
    default:
	aux[0] = '\0';
      break;
  }

  ARG *headcopy = *head;
  if (strcmp(headcopy->data.id, aux) == 0) {
    *head = (*head)->next;
    fprintf(f, "%d\n", *(int*)headcopy->data.val);

    free(headcopy->data.val);
    free(headcopy);
    return;
  }

  ARG *prev = *head;
  headcopy = headcopy->next;
  while (headcopy != NULL) {
    if (strcmp(headcopy->data.id, aux) == 0) {
      prev->next = headcopy->next;
      fprintf(f, "%d\n", *(int*)headcopy->data.val);
      free(headcopy->data.val);
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
    return;
  }

  if ((*head)->next == NULL) {
    fprintf(f, "%s\n", (char*)(*head)->data.val);
    free(*head);
    *head = NULL;
    return;
  }

  ARG *headcopy = *head;
  while (headcopy->next->next != NULL)
    headcopy = headcopy->next;

  fprintf(f, "%s\n", (char*)headcopy->next->data.val);
  free(headcopy->next);
  headcopy->next = NULL;
}

void free_list(ARG *head) {
  ARG* temp;

  while (head != NULL) {
    temp = head;
    head = head->next;
	free(temp->data.val);
    free(temp);
  }
}
