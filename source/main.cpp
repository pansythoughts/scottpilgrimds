// le libraries.
#include "libs.h"

#include "input.h"
#include "sprite.h"
#include "character.h"
#include "background.h"


// le global variables.

bool nfInit;
int frames = 0;

volatile u8* ram = (volatile u8*)0x02000000;
long bytes = 0;
bool debug = false; // change this to allow for debug.

//Prototipos de funciones.//

//Funcion principal.
int main() 
{

	defaultExceptionHandler();
    //Se inicia el 'modo demo' para poder mostrar texto en la segunda pantalla.
	consoleDemoInit();

    //Se inicia todo lo relacionado al sistema de archivos 'NITROFS' y la carpeta 'nitrofiles'
	//(para acceder al sprite).
	nfInit = nitroFSInit(NULL);
	NF_SetRootFolder("NITROFS");

	if(!nfInit){
		std::cout<<"Hubo un error D:";

		return 1;
	}


    //Se inicializa el modo de graficos  en la pantalla de arriba.
	NF_Set2D(0, 0);//(screen, mode).

	//Lo mismo en la segunda pantalla.
    if(second_screen_enabled)
	{
	    NF_Set2D(1, 0);
	    NF_InitSpriteSys(1);
	}


    //Se prepara todo lo relacionado al sistema de sprites.
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0); //'0' se refiere a la pantalla, en este caso es la superior.

	//Se prepara todo lo relacionado al sistema de tiled backgrounds.
	
	NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0); //(screen);

	//Se prepara todo lo relacionado al sistema de sonido.
	NF_InitRawSoundBuffers();


	//Se crea el fondo.
	Background level_bg(FROZEN_SUBURBS);

	level_bg.createBackground();

	//Se carga la musica.
	NF_LoadRawSound("songs/song1", 0, 11025, 0);
	NF_PlayRawSound(0, 127, 64, true, 0);

	//Se crea al personaje "kim".
	Character kim(KIM);
	//Se llenan sus atributos.
	//(luego creare un constructor que incluya toda la wea de abajo).
	//Se llaman a los metodos que alistan el sprite.
	kim.sprite.createSprite();
	kim.shadow.createSprite();


	struct mallinfo info = mallinfo();

	
	//texto pitero :P
	std::cout << "ESTO SE VA A BORRAR NOMAS EMPIECE EL BUCLE AAAAAAAAAAA";


	//Bucle principal
	while (nfInit) 
	{

		//OJO: NO LLAMAR ACA A NF_CreateSprite(), PUES HARA QUE LA ANIMACION SE ENTRECORTE.
		//LLAMARLA UNA SOLA VEZ ANTES DEL LOOP.

		//Se cuentan los frames.
		frames++;

		//Se registran los inputs del usuario.
		scanInput();

		//Metodos para actualizar el sprite cada frame.
		kim.updateCharacter(level_bg.map_scroll_x, level_bg.map_scroll_y);

		level_bg.updateBackground(kim.map_pos_x, kim.map_pos_y);

		info = mallinfo();

		if(debug)
		{


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

		//Update NF OAM Settings
		NF_SpriteOamSet(0);
		NF_SpriteOamSet(1);
		swiWaitForVBlank();

		//Update OAM!
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
	
	}

	return 0;
}

