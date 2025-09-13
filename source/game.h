#pragma once

#include "libs.h"
#include "background.h"
#include "character.h"
#include "enums.h"
#include "input.h"
#include "sound.h"
#include "sprite.h"

class Game
{
    public:
    GAME_STATES game_state = TITLE_SCREEN; // if debugging, change this to whichever state you want when boot.
    GAME_STATES past_game_state;
    LEVELS current_level = L_FROZEN_SUBURBS;
    bool game_state_changed = false;
    bool nfInit;
    bool changing_state = false;
    volatile int frames = 0;
    volatile int frames_state = 0;
    volatile int frames_changing_state = 0;
    volatile int frames_fading = 0;
    int character_selected = KIM;
    bool fading = false;
    int brightness = 0;
    float lvl_scroll_factor = 0;

    bool debug = false; // change this to allow for debug. 
                        // ALSO make sure that there is a VRAM bank set to sub screen BG.

    bool level_init = false;
    bool level_lower_ui_init = false;

    LevelBackground *level_bg;
    Song *level_song;
    Character *level_character;

    // maybe could all of these objects to a LevelLowerUI struct. //
    SimpleSprite *char_letters;
    SimpleSprite *char_image;
    SimpleSprite *char_icon;
    SimpleSprite *scroll_arrow;
    SimpleSprite *scroll_bar;


    void initGame();
    void updateGame();
    void fadeIn(int screen);
    void levelInCutscene();
    void levelLowerUI();
    Game();

};