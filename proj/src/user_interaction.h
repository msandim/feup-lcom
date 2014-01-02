#ifndef _USER_INTERACTION_H_
#define _USER_INTERACTION_H_

/** @defgroup user_interaction User Interaction
 * @{
 *
 * Higher level module that handles with mouse/keyboard interrupts and updates their status,
 * according to the current video mode
 */

// KEYBOARD FUNCTIONS *********************

/**
 * @brief Updates the keyboard status in the program
 *
 * This function is the top layer of the keyboard module.
 * It's called everytime a keyboard interrupt occurs, and using the interrupt handler to know more info about
 * the code received, fills the static variables "last_key_code", "two_byte_flag" and "press_key_flag".
 *
 *
 * @return 1 if a new valid key was received and if we can extract the static variables mentioned
 * above to know more about the key pressed, 0 if not (still processing the interrupts)
 */
int updateKeyboardStatus();

/**
 * @brief Returns the makecode of the last key pressed
 *
 * Returns the static variable "last_key_code", containing the makecode of the last key pressed/released
 *
 * @return the code
 */
unsigned char getKeyboardLastKey();

/**
 * @brief Returns info about the number of number of make/break codes that the current key has
 *
 * If the user presses/releases an arrow key for example (2 byte code), this flag is put to 1.
 * If the user presses/releases a normal key (1 byte code), this flag is put to 0.
 *
 * @return the flag
 */
int getKeyboard2ByteCode();

/**
 * @brief Returns info about press state of the last key pressed/released
 *
 * If the user presses a key, this flag is put to 1.
 * If the user releases a key, this flag is put to 0.
 *
 * @return the flag
 */
int getKeyboardPressState();


// MOUSE FUNCTIONS ***********************

/**
 * @brief Updates the mouse status in the program
 *
 * This function is the top layer of the mouse module.
 * It's called everytime a mouse interrupt occurs, and using the interrupt handler to know more info about
 * the mouse movement or key pressing received, fills the static variables
 * "x_position", "y_position", "RB_pressed", "MB_pressed" and "LB_pressed".
 *
 * @return 1 if a new valid finished packet was received and if we can extract the static variables mentioned
 * above to know more about the mouse current state, 0 if not (still processing the interrupts)
 */
int updateMouseStatus();

/**
 * @brief Returns info the mouse current x position on the current video mode
 *
 * @return the flag
 */
unsigned int getxMousePosition();

/**
 * @brief Returns info the mouse current y position on the current video mode
 *
 * @return the flag
 */
unsigned int getyMousePosition();

/**
 * @brief Returns info the mouse current left button state
 *
 * @return the flag (1 if pressed, 0 if not)
 */
int getMouseLBstate();

/**
 * @brief Returns info the mouse current middle button state
 *
 * @return the flag (1 if pressed, 0 if not)
 */
int getMouseMBstate();

/**
 * @brief Returns info the mouse current right button state
 *
 * @return the flag (1 if pressed, 0 if not)
 */
int getMouseRBstate();

/** @} */

#endif
