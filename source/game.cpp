#include "libs.h"
#include "game.h"

void Game::initGame()
{
    game_state = TITLE_SCREEN;

    defaultExceptionHandler();

    //Se inicia todo lo relacionado al sistema de archivos 'NITROFS' y la carpeta 'nitrofiles'
	//(para acceder al sprite).
	nfInit = nitroFSInit(NULL);
	NF_SetRootFolder("NITROFS");



    //Se inicializa el modo de graficos  en la pantalla de arriba.
	NF_Set2D(0, 0);
    NF_Set2D(1, 0);

    //Se prepara todo lo relacionado al sistema de sprites.
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
    NF_InitSpriteSys(1);

	//Se prepara todo lo relacionado al sistema de tiled backgrounds.
	
	NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);

    NF_InitTextSys(0);
    NF_InitTextSys(1);

	//Se prepara todo lo relacionado al sistema de sonido.
	NF_InitRawSoundBuffers();

}

void Game::updateGame()
{
    scanInput();

    if(game_state_changed)
    frames_state = 0;

    past_game_state = game_state;

    switch(game_state)
    {
        case TITLE_SCREEN:
        if(frames_state == 0)
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

        for(int i = 0; i < 6; i++)
        {
            NF_LoadTiledBg((title_lower + "frame" + std::to_string(i)).c_str()
                        ,(title_lower + "frame" + std::to_string(i)).c_str(), 256, 256);
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
        if(frames_state % 3 == 0 && frames_state <= 15)
        {
            std::string title_upper = "bg/title_screen_0";
            std::string title_lower = "bg/title_screen_1";
            NF_CreateTiledBg(1, 0, (title_lower + "frame" + std::to_string(frames_state / 3)).c_str());
        }


        // switching to next state/screen.
        if(frames_state > 15 && key_pressed & KEY_START)
        changing_state = true;

        if(changing_state)
        {
            if(frames_changing_state == 0)
            NF_PlayRawSound(0, 127, 64, false, 0); // *BEEP* or idk.
            
            // PRESS START flickering.
            if(frames_changing_state % 3 == 0 && frames_changing_state < 45)
            {
                for(int i = 0; i < 2; i++)
                {
                    if(NF_SPRITEOAM[1][i].created)
                    NF_DeleteSprite(1, i);
                }
            }
            else if(frames_changing_state % 5 == 0 && frames_changing_state < 46)
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
            if(frames_changing_state >= 60)
            {
                NF_UnloadSpriteGfx(0);
                NF_UnloadSpritePal(0);
                NF_DeleteTiledBg(0, 0);
                NF_DeleteTiledBg(1, 0);
                NF_UnloadRawSound(0);

                for(int i = 0; i < 2; i++)
                {
                    if(NF_SPRITEOAM[1][i].created)
                    NF_DeleteSprite(1, i);
                }
                NF_ResetTiledBgBuffers();
                game_state = CHARACTER_SELECTION;
            }
        }
        break;

        case CHARACTER_SELECTION:
        if(frames_state == 0)
        {
            changing_state = false;
            frames_changing_state = 0;

            // loads all the assets to RAM.
            NF_LoadRawSound("songs/character_select", 0, 11025, 0);         // character select song.
            NF_LoadTiledBg("bg/curtain", "curtain", 256, 512);              // curtain bg.
            NF_LoadSpriteGfx("sprite/please_select_a_character", 0, 64, 32);// "PLEASE SELECT A CHARACTER".
            NF_LoadSpritePal("palette/please_select_a_character", 0);       // -
            NF_VramSpriteGfx(0, 0, 0, false);                               // -
            NF_VramSpritePal(0, 0, 0);                                      // -
            NF_LoadSpriteGfx("sprite/kim_selection", 1, 64, 64);            // kim select sprite (blurry af).
            NF_LoadSpritePal("palette/kim_selection", 1);                   // -
            NF_VramSpriteGfx(0, 1, 1, false);                               // -
            NF_VramSpritePal(0, 1, 1);    
            NF_LoadSpriteGfx("sprite/character_selection_characters", 2, 64, 32);
            NF_LoadSpritePal("palette/character_selection_characters", 2);
            NF_VramSpriteGfx(0, 2, 2, true);
            NF_VramSpritePal(0, 2, 2);
            NF_LoadSpriteGfx("sprite/character_selection_arrow", 3, 16, 16);
            NF_LoadSpritePal("palette/character_selection_arrow", 3);
            NF_VramSpriteGfx(1, 3, 3, false);
            NF_VramSpritePal(1, 3, 3);

            // song :D
            NF_PlayRawSound(0, 127, 64, true, 0);

            // curtain background.
            NF_CreateTiledBg(0, 0, "curtain");

            // cleans lower half of the background.
            u16* curtain_map = (u16*)(0x6000000 + (NF_TILEDBG_LAYERS[0][0].mapbase << 11));

            // sets to 0 the positions corresponding to the lower half (and a bit more :P).
            for(int i = 768; i < 2048; i++)
            *(curtain_map + i) = 0;

        }

        // curtain animation (only downwards for now).
        if(frames_state <= 10)
        {
            if(frames_state % 4 == 0)
            NF_ScrollBg(0, 0, 0, 100 - (10 * frames_state));
        }
        else
        {

            if(frames_state == 11)
            {
                // "PLEASE SELECT A CHARACTER" text.
                for(int i = 0; i < 3; i++)
                {
                    NF_CreateSprite(0, i, 0, 0, 0, 0);
                    NF_SpriteFrame(0, i, i);
                }
                NF_MoveSprite(0, 0, 65, 60);  // "PLEASE"
                NF_MoveSprite(0, 1, 115, 60); // "SELECT A"
                NF_MoveSprite(0, 2, 90, 75);  // "CHARACTER"
                NF_CreateSprite(0, 3, 1, 1, 95, 95);

                NF_CreateSprite(0, 4, 2, 2, 97, 170);

                NF_CreateSprite(1, 5, 3, 3, 110, 10);
                NF_CreateSprite(1, 6, 3, 3, 110, 170);
                NF_EnableSpriteRotScale(1, 5, 0, true);
                NF_EnableSpriteRotScale(1, 6, 1, true);
                NF_SpriteRotScale(1, 0, 0, 400, 400);
                NF_SpriteRotScale(1, 1, 256, 400, 400);

                // YEAH, EVERYTHING HARD-CODED, COPE.
            }

            if(frames_state % 6 == 0)
            {
                NF_MoveSprite(1, 5, 110, 20);
                NF_MoveSprite(1, 6, 110, 160);
            }
             if (frames_state % 12 == 0)
            {
                NF_MoveSprite(1, 5, 110, 10);
                NF_MoveSprite(1, 6, 110, 170);
            }

            if(key_pressed & KEY_UP)
            {
                if(character_selected > 0)
                character_selected--;
                else
                character_selected = STILLS;
            }
            else if(key_pressed & KEY_DOWN)
            {
                if(character_selected < 4)
                character_selected++;
                else
                character_selected = SCOTT;
            }

            if(key_pressed & KEY_A)
            changing_state = true;

            switch(character_selected)
            {

                case KIM:
                NF_SpriteFrame(0, 4, 2);
                break;

            }   

        }



        if(frames_changing_state >= 30)
        {
            // song stops :c
            soundKill(0);

            // unloads all sprites.
            for(int i = 0; i < 5; i++)
            {
                NF_DeleteSprite(0, i);
                if(i < 1)
                {
                    NF_UnloadSpriteGfx(i);
                    NF_UnloadSpritePal(i);
                }
            }

            // unloads the rest of the stuff.
            NF_UnloadTiledBg("curtain");
            NF_UnloadRawSound(0);
            game_state = LEVEL;
        }
        break;

        case LEVEL:
        if(frames_state == 0)
        {
            changing_state = false;
            frames_changing_state = 0;
            //Se crea el fondo.
	        level_bg = new LevelBackground(FROZEN_SUBURBS);

	        level_bg->createBackground();


	        //Se carga la musica.
	        level_song = new Song(ANOTHER_WINTER);
	        level_song->loadChunkRAM();


	        //Se crea al personaje "kim".
	        level_character = new Character(KIM);
	        level_character->sprite.createSprite();
	        level_character->shadow.createSprite();
        }

        //Metodos para actualizar el sprite cada frame.
		level_character->updateCharacter(level_bg->map_scroll_x, level_bg->map_scroll_y);

		level_bg->updateBackground(level_character->map_pos_x, level_character->map_pos_y);

		level_song->playSong();


    }

    //Update NF OAM Settings
	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);
	swiWaitForVBlank();

	//Update OAM!
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);

    frames++;
    frames_state++;
    
    if(changing_state)
    frames_changing_state++;
    else
    frames_changing_state = 0;

    game_state_changed = (past_game_state != game_state);
}


Game::Game()
{
    initGame();
}