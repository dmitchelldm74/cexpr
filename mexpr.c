#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1000
typedef int val_type;
typedef struct Node {
    val_type val;
    struct Node * Next;
} node_t;
val_type imem[26];
void print_list(node_t* head) {
    node_t* current = head;
    printf("[");
    while (current != NULL) {
        printf("(%c:%d), ", current->val, current->val);
        current = current->Next;
    }
    printf("]\n");
}
node_t* create(val_type val) {
  node_t* head = malloc(sizeof(node_t));
  head->val = val;
  head->Next = NULL;
  return head;
}
void append(node_t* head, val_type val) {
    node_t* current = head;
    while (current->Next != NULL) {
        current = current->Next;
    }
    if(current->val==-1) {
      current->val = val;
      current->Next = NULL;
    } else {
      current->Next = malloc(sizeof(node_t));
      current->Next->val = val;
      current->Next->Next = NULL;
    }
}
void prepend(node_t** head, val_type val) {
  node_t* new = malloc(sizeof(node_t));
  if((*head)->val==-1) {
    (*head)->val = val;
    (*head)->Next = (*head)->Next;
  } else {
    new->val = val;
    new->Next = (*head);
    (*head) = new;
  }
}
node_t* join(node_t* list1, node_t* list2) {
  node_t* current = list1;
  while (current->Next != NULL) {
      current = current->Next;
  }
  current->Next = list2;
  return list1;
}
int len(node_t** list) {
  int i = 0;
  node_t* Next_node = (*list);
  while(Next_node!=NULL) {
    Next_node = Next_node->Next;
    i++;
  }
  return i;
}
val_type pop(node_t** head) {
    int retval = -1;
    node_t * next_node = NULL;
    if (*head == NULL) {
        return -1;
      }
    next_node = (*head)->Next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;
    return retval;
}
val_type pop_end(node_t** head) {
  if (*head == NULL) {
      return -1;
  }
  node_t* current = (*head);
  while (current->Next->Next != NULL) {
      current = current->Next;
  }
  return pop(&current->Next);
}
char str_pop(char* string, int idxToDel) {
  char val = string[idxToDel];
  memmove(&string[idxToDel], &string[idxToDel+1], strlen(string)-idxToDel);
  return val;
}
void str_add(char* s, char* t) {
    size_t len = strlen(t);
    size_t i;
    memmove(s + len, s, strlen(s) + 1);
    for (i = 0; i < len; ++i) {
        s[i] = t[i];
    }
}
void delete_list(node_t *head) {
    node_t  *current = head, *Next = head;
    while (current) {
        Next = current->Next;
        free(current);
        current = Next;
    }
}
void* mgetline(FILE* fp, char* buff) {
  return fgets(buff, BUFSIZE - 1, fp);
}

int get_priority(int c) {
  switch (c) {
    case '+': case '-':
      return 0;
    case '*': case '/': case '^':
      return 1;
    default:
      return 2;
  }
}
int get_type(int c) {
  switch (c) {
    case '!':
      return 1;
    default:
      return 2;
  }
}
int execute_operator(int o, val_type n, node_t* args) {
  switch(o) {
    case '+':
      return n+pop(&args);
    case '-':
      return n-pop(&args);
    case '*':
      return n*pop(&args);
    case '/':
      return n/pop(&args);
    case '^': {
      int x = pop(&args);
      int y = n;
      while((x=x-1)!=0) {
        n *= y;
      }
      return n;
    }
    case '!': {
      int x = n;
      while((x=x-1)!=0) {
        n *= x;
      }
      return n;
    }
    default:
      printf("\nException: Operator Error %d\n    Operator '%c' Does Not Exist!\n", o, o);
      exit(EXIT_FAILURE);
    }
}
val_type execute(node_t* stack) {
  //print_list(stack);
  int n = pop(&stack);
  int o;
  int alen;
  if(stack->Next==NULL) return n;
  while((o=pop_end(&stack))!=-1) {
    //putchar(o);
    switch(o) {
      case ':': {
        int idx = pop(&stack);
        printf("%c[%d] : %d\n", idx, idx-'a', n);
        imem[idx-'a'] = n;
        break;
      } default: {
        node_t* args = create(-1);
        alen = get_type(o);
        while((alen=alen-1)!=0) {
          append(args, pop(&stack));
        }
        n = execute_operator(o, n, args);
      }
      }
  }
  return n;
}
node_t* tokenize(char* expr) {
  node_t* stack = create(-1);
  node_t* op = create(-1);
  char c;
  char* build = malloc(sizeof(char*));
  int blen = 0;
  int expression = 1;
  while(expression) {
    c = str_pop(expr, 0);
    //putchar(c);
    switch(c) {
      case '0' ... '9': {
        build[blen] = c;
        blen += 1;
        build[blen] = '\0';
        break;
      }
      case '(': {
        if(build[0]!='\0') {
          prepend(&stack, atoi(build));
          append(op, '*');
          build[0] = '\0';
          blen = 0;
        }
        append(stack, execute(tokenize(expr)));
        if(expr[0]!=')') {
          printf("\nException: Syntax Error\n    Expected ')' Got '%c'!\n", str_pop(expr,0));
          exit(EXIT_FAILURE);
        }
        break;
      }
      case ')': {
        if(build[0]!='\0') {
          append(stack, atoi(build));
        }
        str_add(expr, ")");
        return join(stack, op);
      }
      case '\0':
        if(build[0]!='\0') {
          append(stack, atoi(build));
        }
        expression = 0;
        break;
      case '\n': case '\r': case ' ':
        break;
      default:
        if(build[0]!='\0') {
          if(!get_priority(c))
            append(stack, atoi(build));
          else prepend(&stack, atoi(build));
          build[0] = '\0';
          blen = 0;
        }
        if(!get_priority(c))
          prepend(&op, c);
        else append(op, c);
        break;
    }
  }
  return join(stack, op);
}

int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1], "r");
    char buff[BUFSIZE];
    char* expr = malloc(sizeof(char*));
    while(mgetline(fp, buff) != NULL)
    {
        printf("%d\n", execute(tokenize(buff)));
    }
    fclose(fp);
    return 1;
}
