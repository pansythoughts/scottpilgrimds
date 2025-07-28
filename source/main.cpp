
// le libraries.
#include "libs.h"
#include "game.h"


// le global variables.


volatile u8* ram = (volatile u8*)0x02000000;
long bytes = 0;

bool debug = false; // change this to allow for debug.

//Prototipos de funciones.//

//Funcion principal.
int main() 
{

	Game game;

    /*
	//Se crea el fondo.
	LevelBackground level_bg(FROZEN_SUBURBS);

	level_bg.createBackground();


	//Se carga la musica.
	Song level_song(ANOTHER_WINTER);
	level_song.loadChunkRAM();


	//Se crea al personaje "kim".
	Character kim(KIM);
	kim.sprite.createSprite();
	kim.shadow.createSprite();


	struct mallinfo info = mallinfo();

	
	//texto pitero :P
	std::cout << "ESTO SE VA A BORRAR NOMAS EMPIECE EL BUCLE AAAAAAAAAAA";
	*/
	//Bucle principal
	while (game.nfInit) 
	{
		game.updateGame();


		/*
		//Se cuentan los frames.
		frames++;

		//Se registran los inputs del usuario.
		scanInput();

		//Metodos para actualizar el sprite cada frame.
		kim.updateCharacter(level_bg.map_scroll_x, level_bg.map_scroll_y);

		level_bg.updateBackground(kim.map_pos_x, kim.map_pos_y);

		level_song.playSong();


		if(debug)
		{


		// shitty ram used calculation lmao.
		for (int i = 0; i < (0x023FFFFF - 0x02000000); i++)
        {
        if (ram[i] != 0)
            bytes++;
		}
	    




		//CONSOLE DEBUG//
		
		consoleClear();
		
		std::cout << "\nSTART POS: " << kim.jump_start_pos
		          << "\nCURRENT SCREEN POS Y: " << kim.sprite.screen_pos_y
				  << "\nCURRENT SCREEN POS X: " << kim.sprite.screen_pos_x
				  << "\nCURRENT MAP POS Y: " << kim.map_pos_y
				  << "\nCURRENT MAP POS X: " << kim.map_pos_x
				  << "\nSTATUS PRINCIPAL: " << kim.primary_status
				  << "\nSTATUS SECUNDARIO: " << kim.secondary_status
				  << "\nANIM STATUS: " << kim.sprite.anim_status
				  << "\nKIMSPRITE_ID[ANIM_STATUS]: " << kim.sprite.sprite_id[kim.sprite.anim_status]
				  << "\nLANDEFFECTSPRITE_ID[ANIM_STATUS]: " << kim.effects[EFF_LAND_EFFECT].sprite_id[kim.effects[EFF_LAND_EFFECT].anim_status]
				  << "\nKIMPALETTE: " << kim.sprite.pal_id
				  << "\nLANDEFFECTPALETTE: " << kim.effects[EFF_LAND_EFFECT].pal_id
				  << "\nFRAMES SALTANDO: " << kim.frames_jumping
				  << "\nCAMBIO ESTADO PRIMARIO: " << kim.primary_status_changed
				  << "\nALENTANDOSE: " << kim.slowing_down
				  << "\nCAMBIO ANIMACION: " << kim.sprite.anim_status_changed
				  << "\nPUEDE MOVERSE: " << kim.can_move
				  << "\nESQUIVANDO: " << kim.dodging
				  << "\nSALTANDO: " << kim.jumping
				  << "\nMAP_SCROLL X: " << level_bg.map_scroll_x
				  << "\nMAP_SCROLL Y: " << level_bg.map_scroll_y
				  << "\nCHUNK_SCROLL X: " << level_bg.chunk_scroll_x
				  << "\nCHUNK_SCROLL Y: " << level_bg.chunk_scroll_y
				  << "\nRAMLEFT: " << info.fordblks
				  << "\nRAMLEFT2: " << bytes
				  << "\nFRAMES: " << frames;
		bytes = 0;
	}
		if(second_screen_enabled)
		{
		if(keysDown() & KEY_X)
		kim.sprite.changeScreen();
		}
        */
	
	}

	return 0;
}

