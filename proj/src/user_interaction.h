#ifndef _USER_INTERACTION_H_
#define _USER_INTERACTION_H_

/*typedef int bool;
#define true 1
#define false 0*/

// Updates the keyboard status. returns 1 if a key can be read, 0 if not
int updateKeyboardStatus();

// Updates the mouse status. returns 1 if the current packet is finished, 0 if not
int updateMouseStatus();

// Returns the code of the last key pressed or released (it returns ALWAYS the makecode,
// even if the key is released)
unsigned char getKeyboardLastKey();

// Returns 1 if the last key has a 2 byte code (arrows for example), 0 if not
int getKeyboard2ByteCode();

// Returns 1 if the key was pressed (makecode), or 0 if the key is released (breakcode)
int getKeyboardPressState();

// Returns the x_position of the mouse
unsigned int getxMousePosition();

// Returns the y_position of the mouse
unsigned int getyMousePosition();

// Returns 1 if left button of the mouse is on
int getMouseLBstate();

// Returns 1 if the middle button of the mouse is on
int getMouseMBstate();

// Returns 1 if the right button of the mouse is on
int getMouseRBstate();

#endif
