#ifndef _CENTRAL_MODULE_H_
#define _CENTRAL_MODULE_H_

/** @defgroup central_module Central Module
 * @{
 *
 * Module that runs the devices and handles interrupts. Controles the other modules as well.
 */

typedef enum {INTRO, MENU, GALLERY, DRAW_SINGULAR, DRAW_MULTI} program_state;

/**
 * @brief Initiates the program
 * Calls several routines that alloc memory, and finally some that free that exact memory
 */
void programInit();

/**
 * @brief Changes the program's current state
 *
 * Change's the program's current state and prepares each state if needed
 */
void changeProgramState(program_state new_state);

/**
 * @brief Run's the devices and the interrupts associated to them
 *
 * Main function that manages the interrupts and the program's states
 *
 */
void runDevices();

/**
 * @brief Handles Intro Screen Keyboard Events
 *
 * Handles the key events, turning them into actions in the program
 *
 * @return 1 if a key is pressed, 0 if not
 */
int keyboardIntroEvent();

/**
 * @brief Handles Menu Mouse events
 *
 * Handles the key events, turning them into actions in the program
 *
 * @return 1 if a key is pressed, 0 if not
 */
int mouseMenuEvent();

/** @} */

#endif
