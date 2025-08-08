
// le libraries.
#include "libs.h"
#include "game.h"


// le global variables.


volatile u8* ram = (volatile u8*)0x02000000;
long bytes = 0;


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
	    */

	
	
	}

	return 0;
}

