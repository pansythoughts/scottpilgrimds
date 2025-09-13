#include "libs.h"
#pragma once

// MAYBE could add all of these things to the Game class...

extern int key_pressed;
extern int key_held;
extern int key_current;
extern int key_down_repeat;
extern int key_up;
extern int last_dpad_key;
extern int second_screen_enabled;
extern touchPosition touch;

void scanInput();