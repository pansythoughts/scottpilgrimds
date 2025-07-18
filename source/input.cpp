#include "libs.h"
#include "input.h"

int key_pressed = 0;
int key_held = 0;
int key_current = 0;
int key_down_repeat = 0;
int last_dpad_key = 0;
int key_up = 0;
int second_screen_enabled = false; // <-- change this if wanted !!1

// le input scan.
void scanInput()
{
    scanKeys();

    if(keysDown() & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT))
	last_dpad_key = keysDown();


	key_pressed = keysDown();
	key_held = keysHeld();
	key_current = keysCurrent();
	key_up = keysUp();
}