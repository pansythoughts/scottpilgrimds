
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

	while (game.nfInit) 
	{
		game.updateGame();


		
		

        /*
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

