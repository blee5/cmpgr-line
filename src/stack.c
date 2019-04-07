#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "stack.h"

/*
 * Creates a new stack with an identity matrix
 */
struct stack *new_stack()
{
    struct stack *s;
    struct matrix **m;
    struct matrix *i;

    s = malloc(sizeof(struct stack));
    m = malloc(STACK_SIZE * sizeof(struct matrix *));
    i = ident(4);
    s->size = STACK_SIZE;
    s->top = 0;
    s->data = m;
    s->data[s->top] = i;

    return s;
}

void push(struct stack *s, struct matrix *m)
{
    if (s->top == s->size - 1)
    {
        s->data = realloc(s->data, (s->size + STACK_SIZE) *
                                    sizeof(struct matrix *));
        s->size += STACK_SIZE;
    }
    s->top++;
    s->data[s->top] = m;
}

struct matrix *peek(struct stack *s)
{
    return s->data[s->top];
}

void pop(struct stack *s)
{
    free_matrix(s->data[s->top]);
    s->top--;
}

void free_stack(struct stack *s)
{
    int i;
    for (i = 0; i <= s->top; i++)
    {
        free_matrix(s->data[i]);
    }
    free(s->data);
    free(s);
}

void print_stack(struct stack *s)
{
    int i;
    for (i = 0; i <= s->top; i++)
    {
        print_matrix(s->data[i]);
        printf("\n");
    }
}
