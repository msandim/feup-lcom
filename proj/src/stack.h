#ifndef _STACK_H_
#define _STACK_H_

typedef struct {
	int *buf;
	int top;
	int size;
} stack;

stack *new_stack(int ini_size);

void delete_stack(stack *s);

int push_stack (stack *s, int obj);

int pop_stack (stack *s, int* obj);

#endif
