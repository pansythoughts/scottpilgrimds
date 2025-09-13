#include "libs.h"
#include "game.h"

void Game::initGame()
{

    // when an excepction occurs, at least it puts out some text lol.
    defaultExceptionHandler();

    // nitrofiles init (game files).
    nfInit = nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // graphic modes init (mode 0 for both screens).
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    // sprite system init.
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0);
    NF_InitSpriteSys(1);

    // tiled background system init.
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);

    // text system init.
    NF_InitTextSys(0);
    NF_InitTextSys(1);

    // sound system init.
    NF_InitRawSoundBuffers();

    if (debug)
        consoleDemoInit();
}

void Game::updateGame()
{
    scanInput();

    if (game_state_changed)
        frames_state = 0;

    past_game_state = game_state;

    switch (game_state)
    {
    case TITLE_SCREEN:
        if (frames_state == 0)
        {
            changing_state = false;
            frames_changing_state = 0;
            std::string title_upper = "bg/title_screen_0";
            std::string title_lower = "bg/title_screen_1";

            // loads all the assets to RAM.
            NF_LoadSpriteGfx("sprite/press_start", 0, 64, 32);
            NF_LoadSpritePal("palette/press_start", 0);
            NF_VramSpriteGfx(1, 0, 0, false);
            NF_VramSpritePal(1, 0, 0);
            NF_LoadTiledBg(title_upper.c_str(), title_upper.c_str(), 256, 256);
            NF_LoadRawSound("sfx/press_start_sfx", 0, 22050, 0);

            for (int i = 0; i < 6; i++)
            {
                NF_LoadTiledBg((title_lower + "frame" + std::to_string(i)).c_str(), (title_lower + "frame" + std::to_string(i)).c_str(), 256, 256);
            }

            // shows all the assets.

            // PRESS START //
            NF_CreateSprite(1, 0, 0, 0, 10, 55);
            NF_CreateSprite(1, 1, 0, 0, 10, 95);
            NF_EnableSpriteRotScale(1, 0, 0, false);
            NF_EnableSpriteRotScale(1, 1, 1, false);
            NF_SpriteRotScale(1, 0, 128, 256, 256);
            NF_SpriteRotScale(1, 1, 128, 256, 256);
            NF_SpriteFrame(1, 1, 1);

            // upper/left image. //
            NF_CreateTiledBg(0, 0, title_upper.c_str());
        }
        // animates the title screen.
        if (frames_state % 3 == 0 && frames_state <= 15)
        {
            std::string title_upper = "bg/title_screen_0";
            std::string title_lower = "bg/title_screen_1";
            NF_CreateTiledBg(1, 0, (title_lower + "frame" + std::to_string(frames_state / 3)).c_str());
        }

        // switching to next state/screen.
        if (frames_state > 15 && (key_pressed & KEY_START || key_pressed & KEY_TOUCH))
            changing_state = true;

        if (changing_state)
        {
            if (frames_changing_state == 0)
                NF_PlayRawSound(0, 127, 64, false, 0); // *BEEP* or idk.

            // PRESS START flickering.
            if (frames_changing_state % 3 == 0 && frames_changing_state < 45)
            {
                for (int i = 0; i < 2; i++)
                {
                    if (NF_SPRITEOAM[1][i].created)
                        NF_DeleteSprite(1, i);
                }
            }
            else if (frames_changing_state % 5 == 0 && frames_changing_state < 46)
            {
                NF_CreateSprite(1, 0, 0, 0, 10, 55);
                NF_CreateSprite(1, 1, 0, 0, 10, 95);
                NF_EnableSpriteRotScale(1, 0, 0, false);
                NF_EnableSpriteRotScale(1, 1, 1, false);
                NF_SpriteRotScale(1, 0, 128, 256, 256);
                NF_SpriteRotScale(1, 1, 128, 256, 256);
                NF_SpriteFrame(1, 1, 1);
            }
            // when finally switching/changing state...
            if (frames_changing_state >= 60)
            {
                NF_UnloadSpriteGfx(0);
                NF_UnloadSpritePal(0);
                NF_DeleteTiledBg(0, 0);
                NF_DeleteTiledBg(1, 0);
                NF_UnloadRawSound(0);

                for (int i = 0; i < 2; i++)
                {
                    if (NF_SPRITEOAM[1][i].created)
                        NF_DeleteSprite(1, i);
                }
                NF_ResetTiledBgBuffers();
                game_state = CHARACTER_SELECTION;
            }
        }
        break;

    case CHARACTER_SELECTION:
        if (frames_state == 0)
        {
            changing_state = false;
            frames_changing_state = 0;

            // loads all the assets to RAM.
            NF_LoadRawSound("songs/character_select", 0, 11025, 0);          // character select song.
            NF_LoadTiledBg("bg/curtain", "curtain", 256, 512);               // curtain bg.
            NF_LoadSpriteGfx("sprite/please_select_a_character", 0, 64, 32); // "PLEASE SELECT A CHARACTER".
            NF_LoadSpritePal("palette/please_select_a_character", 0);        // -
            NF_VramSpriteGfx(0, 0, 0, false);                                // -
            NF_VramSpritePal(0, 0, 0);                                       // -
            NF_LoadSpriteGfx("sprite/kim_selection", 1, 64, 64);             // kim select sprite (blurry af).
            NF_LoadSpritePal("palette/kim_selection", 1);                    // -
            NF_VramSpriteGfx(0, 1, 1, false);                                // -
            NF_VramSpritePal(0, 1, 1);
            NF_LoadSpriteGfx("sprite/character_selection_characters", 2, 64, 32);
            NF_LoadSpritePal("palette/character_selection_characters", 2);
            NF_VramSpriteGfx(0, 2, 2, true);
            NF_VramSpritePal(0, 2, 2);
            NF_LoadSpriteGfx("sprite/character_selection_arrow", 3, 16, 16);
            NF_LoadSpritePal("palette/character_selection_arrow", 3);
            NF_VramSpriteGfx(1, 3, 3, false);
            NF_VramSpritePal(1, 3, 3);
            NF_LoadTextFont("fonts/font1", "font1", 256, 256, 0);

            // song :D
            NF_PlayRawSound(0, 127, 64, true, 0);

            // curtain background.
            NF_CreateTiledBg(0, 0, "curtain");

            // cleans lower half of the background.
            u16 *curtain_map = (u16 *)(0x6000000 + (NF_TILEDBG_LAYERS[0][0].mapbase << 11));

            // sets to 0 the positions corresponding to the lower half (and a bit more :P).
            for (int i = 896; i < 2048; i++)
                *(curtain_map + i) = 0;

            NF_CreateTextLayer(0, 1, 0, "font1");
            NF_DefineTextColor(0, 1, 0, 31, 31, 31); // 0 = WHITE
            NF_DefineTextColor(0, 1, 1, 24, 11, 5);  // 1 = ORANGE
            NF_DefineTextColor(0, 1, 2, 30, 22, 3);  // 2 = YELLOW-ORANGE

            NF_SetTextColor(0, 1, 1);
        }

        NF_UpdateTextLayers();

        // curtain animation (only downwards for now).
        if (frames_state <= 10)
        {
            if (frames_state % 4 == 0)
                NF_ScrollBg(0, 0, 0, 90 - (10 * frames_state));
        }
        else
        {
            // once the curtain is totally settled...
            if (frames_state == 11)
            {
                // "PLEASE SELECT A CHARACTER" text.
                for (int i = 0; i < 3; i++)
                {
                    NF_CreateSprite(0, i, 0, 0, 0, 0);
                    NF_SpriteFrame(0, i, i);
                }
                NF_MoveSprite(0, 0, 65, 65);  // "PLEASE"
                NF_MoveSprite(0, 1, 115, 65); // "SELECT A"
                NF_MoveSprite(0, 2, 90, 80);  // "CHARACTER"
                NF_CreateSprite(0, 3, 1, 1, 95, 95);

                NF_CreateSprite(0, 4, 2, 2, 97, 170);

                NF_CreateSprite(1, 5, 3, 3, 110, 10);
                NF_CreateSprite(1, 6, 3, 3, 110, 170);
                NF_EnableSpriteRotScale(1, 5, 0, true);
                NF_EnableSpriteRotScale(1, 6, 1, true);
                NF_SpriteRotScale(1, 0, 0, 400, 400);
                NF_SpriteRotScale(1, 1, 256, 400, 400);

                NF_WriteText(0, 1, 13, 23, "LEVEL 1"); // i'm not coding actual character levels at this point :P.

                // YEAH, EVERYTHING HARD-CODED, COPE.
            }

            // arrows movement.
            if (frames_state % 6 == 0)
            {
                NF_MoveSprite(1, 5, 110, 5);
                NF_MoveSprite(1, 6, 110, 165);
            }
            if (frames_state % 12 == 0)
            {
                NF_MoveSprite(1, 5, 110, 0);
                NF_MoveSprite(1, 6, 110, 170);
            }

            if (key_pressed & KEY_UP || (touch.px > 115 && touch.px < 140 && touch.py > 10 && touch.py < 30))
            {
                // selection logic. (not really used for now.)
                if (character_selected > 0)
                    character_selected--;
                else
                    character_selected = STILLS;

                NF_CreateTextLayer(1, 1, 0, "font1");
                NF_WriteText(1, 1, 1, 12, "ONLY KIM AVAILABLE FOR NOW  :C");
            }
            else if (key_pressed & KEY_DOWN || (touch.px > 115 && touch.px < 140 && touch.py > 165 && touch.py < 185))
            {
                if (character_selected < 4)
                    character_selected++;
                else
                    character_selected = SCOTT;

                NF_CreateTextLayer(1, 1, 0, "font1");
                NF_WriteText(1, 1, 1, 12, "ONLY KIM AVAILABLE FOR NOW  :C");
            }

            if (key_pressed & KEY_A)
                changing_state = true;

            switch (character_selected)
            {

            case KIM:
                NF_SpriteFrame(0, 4, 2);
                break;
            }
        }

        if (frames_changing_state >= 30)
        {
            // song stops :c
            soundKill(0);

            // unloads all sprites.
            // OKAY, this is horribly hardcoded, maybe will be switching all this sht to SimpleSprite objects.
            for (int i = 0; i < 5; i++)
            {
                NF_DeleteSprite(0, i);
                if (i > 2)
                    NF_DeleteSprite(1, i + 2);

                if (i < 4)
                {
                    NF_UnloadSpriteGfx(i);
                    NF_UnloadSpritePal(i);
                    if (i < 3)
                    {
                        NF_FreeSpriteGfx(0, i);
                        NF_SPRPALSLOT[0][i].inuse = false;
                    }
                    else
                    {
                        NF_FreeSpriteGfx(1, i);
                        NF_SPRPALSLOT[1][i].inuse = false;
                    }
                }
            }

            // unloads the rest of the stuff.
            NF_UnloadTiledBg("curtain");
            NF_UnloadRawSound(0);
            NF_ClearTextLayer(0, 1);

            // finally changes to le level.
            game_state = LEVEL;
        }
        break;

    case LEVEL:
        if (frames_state == 0)
        {
            setBrightness(3, -16);
            changing_state = false;
            frames_changing_state = 0;

            level_bg = new LevelBackground(B_FROZEN_SUBURBS);
            level_bg->createBackground();

            level_song = new Song(ANOTHER_WINTER);
            level_song->loadChunkRAM();

            level_character = new Character(KIM);
            level_character->sprite.createSprite();
            level_character->shadow.createSprite();
        }

        if (frames_state < 300)
        {
            levelInCutscene();
        }
        else
        {
            if (frames_state >= 300)
                levelLowerUI();

            // Metodos para actualizar el sprite cada frame.
            level_character->updateCharacter(level_bg->map_scroll_x, level_bg->map_scroll_y);

            level_bg->updateBackground(level_character->map_pos_x, level_character->map_pos_y);

            level_song->playSong();
        }
    }

    // CONSOLE DEBUG//

    if (debug)
    {
        consoleClear();

        std::cout << "\nSTART POS: " << level_character->jump_start_pos
                  << "\nCURRENT SCREEN POS Y: " << level_character->sprite.screen_pos_y
                  << "\nCURRENT SCREEN POS X: " << level_character->sprite.screen_pos_x
                  << "\nCURRENT MAP POS Y: " << level_character->map_pos_y
                  << "\nCURRENT MAP POS X: " << level_character->map_pos_x
                  << "\nSTATUS PRINCIPAL: " << level_character->primary_status
                  << "\nSTATUS SECUNDARIO: " << level_character->secondary_status
                  << "\nANIM STATUS: " << level_character->sprite.anim_status
                  << "\nKIMSPRITE_ID[ANIM_STATUS]: " << level_character->sprite.sprite_id[level_character->sprite.anim_status]
                  << "\nLANDEFFECTSPRITE_ID[ANIM_STATUS]: " << level_character->effects[EFF_LAND_EFFECT].sprite_id[level_character->effects[EFF_LAND_EFFECT].anim_status]
                  << "\nKIMPALETTE: " << level_character->sprite.pal_id
                  << "\nLANDEFFECTPALETTE: " << level_character->effects[EFF_LAND_EFFECT].pal_id
                  << "\nFRAMES SALTANDO: " << level_character->frames_jumping
                  << "\nCAMBIO ESTADO PRIMARIO: " << level_character->primary_status_changed
                  << "\nALENTANDOSE: " << level_character->slowing_down
                  << "\nCAMBIO ANIMACION: " << level_character->sprite.anim_status_changed
                  << "\nPUEDE MOVERSE: " << level_character->can_move
                  << "\nESQUIVANDO: " << level_character->dodging
                  << "\nSALTANDO: " << level_character->jumping
                  << "\nMAP_SCROLL X: " << level_bg->map_scroll_x
                  << "\nMAP_SCROLL Y: " << level_bg->map_scroll_y
                  << "\nCHUNK_SCROLL X: " << level_bg->chunk_scroll_x
                  << "\nCHUNK_SCROLL Y: " << level_bg->chunk_scroll_y
                  << "\nFRAMES_FADING: " << frames_fading
                  << "\nFRAMES: " << frames;
    }

    // Update NF OAM Settings
    NF_SpriteOamSet(0);
    NF_SpriteOamSet(1);
    swiWaitForVBlank();

    // Update OAM!
    oamUpdate(&oamMain);
    oamUpdate(&oamSub);

    frames++;
    frames_state++;

    if (changing_state)
        frames_changing_state++;
    else
        frames_changing_state = 0;

    game_state_changed = (past_game_state != game_state);
}

void Game::fadeIn(int screen)
{

    if (!fading)
        fading = true;

    brightness = -16 + (frames_fading * 16) / FADE_DURATION;

    setBrightness(screen, brightness);
    frames_fading++;

    if (frames_fading >= FADE_DURATION)
        fading = false;
}

void Game::levelInCutscene()
{

    if (frames_fading < FADE_DURATION)
        fadeIn(3);

    if (frames_state == 0)
    {
        level_bg->map_scroll_y = 46;
        level_bg->chunk_scroll_y = 46;
        level_bg->scrollNoLoop(level_bg->screen, level_bg->layers[LEFT], level_bg->chunk_scroll_x, level_bg->chunk_scroll_y);
        level_character->sprite.anim_status = A_RUN;
        level_character->sprite.anim_status_changed = true;
        level_character->sprite.screen_pos_x = -60;
        level_character->sprite.screen_pos_y = 70;
        level_character->shadow.screen_pos_x = level_character->sprite.screen_pos_x;
        level_character->shadow.screen_pos_y = level_character->sprite.screen_pos_y + 5;
        level_character->sprite.updateSprite();
        level_character->shadow.updateSprite();
    }

    if (frames_state >= 120 && frames_state < 160)
    {
        level_character->sprite.screen_pos_x += 3;
        level_character->shadow.screen_pos_x = level_character->sprite.screen_pos_x;
        level_character->shadow.screen_pos_y = level_character->sprite.screen_pos_y + 5;
        level_character->sprite.updateSprite();
        level_character->shadow.updateSprite();
        level_character->sprite.anim_status_changed = false;
    }
    if (frames_state >= 158)
    {
        if (frames_state == 158)
            level_character->sprite.anim_status_changed = true;
        else
            level_character->sprite.anim_status_changed = false;

        level_character->map_pos_x = level_character->sprite.screen_pos_x;
        level_character->map_pos_y = level_character->sprite.screen_pos_y + level_bg->map_scroll_y;
        level_character->sprite.anim_status = A_IDLE;
        level_character->sprite.updateSprite();
    }
}

void Game::levelLowerUI()
{

    switch (level_character->char_id)
    {
    case KIM:

        if (!level_lower_ui_init)
        {
            level_lower_ui_init = true;

            char_letters = new SimpleSprite(1, 64, 64, "kim_pine_letters", false, 2, false);
            char_image = new SimpleSprite(1, 64, 64, "kim_pine_image", false, 2, false);
            char_icon = new SimpleSprite(1, 32, 32, "kim_icon", false, 1, false);
            scroll_arrow = new SimpleSprite(1, 8, 8, "scroll_arrow", false, 1, false);
            scroll_bar = new SimpleSprite(1, 16, 16, "scroll_bar", false, 23, true);

            char_image->createSprite(150, 120);
            char_letters->createSprite(10, 120);
            scroll_arrow->createSprite(20, 40);
            scroll_bar->createSprite(20, 50);
            char_icon->createSprite(13, 20);

            NF_MoveSprite(1, char_image->oam_id[1], char_image->pos_x + 45, char_image->pos_y);
            NF_MoveSprite(1, char_letters->oam_id[1], char_letters->pos_x + 64, char_image->pos_y);

            for (int i = 0; i < scroll_bar->num_of_parts; i++)
            {
                NF_MoveSprite(1, scroll_bar->oam_id[i], scroll_bar->pos_x + (i * (scroll_bar->size_x - 7)), scroll_bar->pos_y);

                if (i == 0 || i == (scroll_bar->num_of_parts - 1))
                {
                    // set to part/frame 0 the first and last parts.
                    NF_SpriteFrame(1, scroll_bar->oam_id[i], 0);

                    if (i == (scroll_bar->num_of_parts - 1))
                    {
                        NF_HflipSprite(1, scroll_bar->oam_id[i], true);
                    }
                }
                else
                    NF_SpriteFrame(1, scroll_bar->oam_id[i], 1);
            }
        }

        switch (current_level)
        {
        case L_FROZEN_SUBURBS:
            lvl_scroll_factor = 0.027f;
            break;
        }

        scroll_arrow->pos_x = 20 + (level_character->map_pos_x * lvl_scroll_factor);
        char_icon->pos_x = 13 + (level_character->map_pos_x * lvl_scroll_factor);

        NF_MoveSprite(1, scroll_arrow->oam_id[0], scroll_arrow->pos_x, scroll_arrow->pos_y);
        NF_MoveSprite(1, char_icon->oam_id[0], char_icon->pos_x, char_icon->pos_y);

        break;
    }
}

Game::Game()
{
    initGame();
}