#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "stack.h"

stack *new_stack(int ini_size) {
	stack * s = (stack*) malloc(sizeof(stack));

	s->buf = (int*) malloc (ini_size * sizeof(int));

	if( s == NULL || s->buf == NULL ) {
		free(s);
		return NULL;
	}

	s->size = ini_size;
	s->top = -1;

	return s;
}

void delete_stack(stack *s) {
	free (s->buf);
	free(s);
}

int push_stack (stack *s, int obj) {

	// if the last position of the vector is the top (we cant fit any more)
	if (s->top >= s->size - 1)
		return 1;

	s->top++;
	s->buf[s->top] = obj;

	printf ("Object %u pushed into stack!\n",obj);

	return 0;
}

int pop_stack (stack *s, int* obj) {

	if (s->top == -1)
		return 1;

	*obj = s->buf[s->top];

	s->top--;

	printf ("Object %u popped out of stack!\n",*obj);
	return 0;
}
