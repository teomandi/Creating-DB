#include <stdlib.h>
#include <stdio.h>

#include "defn.h"
#include "AM.h"
#define STACK_MAX 100


typedef struct Stack
{
	int data[STACK_MAX];
	int size;
}Stack;

int Stack_Top(Stack *S)
{
    if (S->size == 0) {
        printf("Error: stack empty\n");
        return -1;
    } 

    return S->data[S->size-1];
}

void Stack_Push(Stack *S, int d)
{
    if (S->size < STACK_MAX)
        S->data[S->size++] = d;
    else
        printf("Error: stack full\n");
}

int Stack_Pop(Stack *S)
{
    if (S->size == 0)
        printf("Error: stack empty\n");
    else{
        int x = S->data[S->size];
        S->size--;
        return x ;
    }
}