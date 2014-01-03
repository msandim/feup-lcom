#ifndef _STACK_H_
#define _STACK_H_

/** @defgroup stack Stack
 * @{
 *
 * Module that implements a stack structure of ints and methods that operate with this structure
 */

/** @name Stack Struct*/
/**@{
 *
 * Stack of ints - structure
 */
typedef struct {
	int *buf; /**< @brief pointer to the array of ints (base of the structure) */
	int top; /**< @brief Index of the buf array that indicates the top of the stack */
	int size; /**< @brief Size of the buf array (size of the stack) */
} Stack;

/** @}  */

/**
 * @brief Structure constructor
 * @param ini_size max number of ints that the stack will have
 *
 * @return the stack created
 */
Stack *new_stack(int ini_size);

/**
 * @brief Structure destructor
 * @param s pointer to the stack to destroy (free the memory allocated for it)
 *
 */
void delete_stack(Stack *s);

/**
 * @brief Stack method - Push
 * @param s pointer to the stack to push an element to
 * @param obj int to push to the top of the stack
 *
 * @return 0 on success, non-zero otherwise
 *
 */
int push_stack (Stack *s, int obj);

/**
 * @brief Stack method - Pop
 * @param s pointer to the stack to pop an element from
 * @param obj pointer to the int variable where to pop the element to
 *
 * @return 0 on success, non-zero otherwise
 *
 */
int pop_stack (Stack *s, int* obj);

/** @} */

#endif
