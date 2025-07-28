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
    GAME_STATES game_state;
    GAME_STATES past_game_state;
    bool game_state_changed = false;
    bool nfInit;
    bool changing_state = false;
    volatile int frames = 0;
    volatile int frames_state = 0;
    volatile int frames_changing_state = 0;
    int character_selected = KIM;

    bool level_init = false;

    LevelBackground *level_bg;
    Song *level_song;
    Character *level_character;


    void initGame();
    void updateGame();
    Game();

};