#ifndef _CENTRAL_MODULE_H_
#define _CENTRAL_MODULE_H_

typedef enum {INTRO, MENU, GALLERY, DRAW_SINGULAR, DRAW_MULTI} program_state;

void programInit();

void changeProgramState(program_state new_state);

void runDevices();

int keyboardIntroEvent();

int mouseMenuEvent();

#endif
