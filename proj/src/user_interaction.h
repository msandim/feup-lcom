#ifndef _USER_INTERACTION_H_
#define _USER_INTERACTION_H_

// Updates the mouse status. returns 1 if the current packet is finished, 0 if not
int updateMouseStatus();

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
