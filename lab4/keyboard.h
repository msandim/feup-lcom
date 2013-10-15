#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

// constants to use in keyboard

#define DELAY_US 20000

#define KBD_IRQ 1

int keyboard_subscribe_int();

int keyboard_unsubscribe_int();

void keyboard_int_handler();

#endif
