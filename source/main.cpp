// le libraries.
#include "libs.h"

#include "sprite.h"

// le constants.




// le global variables.

bool nfInit;
bool second_screen_enabled = false; //Cambiar esto para permitir todo lo de cambiar pantallas.
int key_pressed = 0;
int key_held = 0;
int key_current = 0;
int key_down_repeat = 0;
int key_up = 0;
int last_dpad_key = 0;
int frames = 0;

// le classes

class Character;

class Character
{
	public:
	char name[10];
	int map_pos_x = 0;
	int map_pos_y = 0;
	float vel_x = 0;
	float vel_y = 0;
	float jump_vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;
	float jump_acc_y = 0;
	int jump_height = 0;
	int primary_status = 0; //0.IDLE,1.WALK,2.RUN.
	int secondary_status = 0; //0.NONE,1.JUMP.
	int past_primary_status = 0;
	int past_secondary_status = 0;
	bool primary_status_changed = false;
	bool secondary_status_changed = false;
	bool can_move = false;
	bool slowing_down = false;
	int frames_moving = 0;
	int frames_jumping = 0;
	bool jumping = false;
	bool dodging = false;
	int jump_start_pos = 0;
	int jump_final_pos = 0;
	bool can_dtap = false;
	bool can_run = false;
	bool can_dodge = false;
	int last_tap_time_x = 0;  
	int last_tap_time_y = 0;  
    int double_tap_threshold = 15;
	Sprite sprite;
	Sprite shadow;

	// le class methods.//

    void setupShadow(){
		shadow.type = SHADOW;
		shadow.sprite_name = sprite.sprite_name + "_shadow";
		shadow.id = sprite.id + 1;
	}

	// calculates the current screen position of the character.
	void mapToScreenPos(){
		//luego hare una version mas aca que permita el scrolleo :D
		sprite.screen_pos_x = map_pos_x;
		sprite.screen_pos_y = map_pos_y + jump_height;
	
	}

	// le jump wooo.
	void Jump(){

		if(!jumping)
		{
		sprite.anim_frame = 0;
		vel_x = 0;
		jumping = true;

		// applies initial velocity.
		if(!dodging)
		jump_vel_y = -6.7;
		else
		jump_vel_y = -4.7;

		}
		
		if(jumping)
		{
		frames_jumping++;
		}

		if(frames_jumping >= 1 && jumping)
		{
		// jump physics.
		jump_acc_y = 0.3;
		jump_vel_y += jump_acc_y;
		jump_height += jump_vel_y;
	    }

		// controls animation frames (so that the animation happens as in the original game :p).
		if(jump_vel_y <= 0 && jump_vel_y >= -0.5 && !dodging)
		{
			if(sprite.anim_frame == 1 || sprite.anim_frame == 3)
			sprite.anim_direction *= -1;
		}
		else
			sprite.anim_direction = 1;

		// same as before.
		if(jump_height <= 2 && jump_height >= 6 && frames_jumping > 20)
		sprite.anim_frame = 12;

	    if(jump_height >= 0)
		{
		// stops jumping.
		sprite.anim_frame = 12;
		frames_jumping = 0;
		jumping = false;
		}
		
		// scales the shadow.
		if(frames_jumping % 10 == 0)
		{
		shadow.scale_x = 256 + (2.5f * jump_height);
		shadow.scale_y = 256 + (2.5f * jump_height);
		}
	}
	

	// manages 'double tap' mechanic.
	void doubleTap()
	{
		if(key_pressed & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN))
		{
		// horizontal (for running).
		if(key_pressed & (KEY_LEFT | KEY_RIGHT))
		{
		if(frames - last_tap_time_x <= double_tap_threshold && !jumping)
		{
            can_run = true;
            can_dtap = false;
        } 
		else 
		{
            can_run = false;
            can_dtap = true;
            last_tap_time_x = frames;
        }
	    }

		// vertical (for dodging.)
		else if(key_pressed & (KEY_UP | KEY_DOWN))
		{
		if(frames - last_tap_time_y <= double_tap_threshold && !jumping)
		{
            can_dodge = true;
            can_dtap = false;
        } 
		else 
		{
            can_dodge = false;
            can_dtap = true;
            last_tap_time_y = frames;
        }
	    }
        }

        if(frames - last_tap_time_x > double_tap_threshold && frames - last_tap_time_y > double_tap_threshold )
        can_dtap = false;
		
        // manages the different states based on the double tap info.
		if(can_run && key_held & (KEY_LEFT | KEY_RIGHT) && last_dpad_key != (KEY_UP | KEY_DOWN))
		primary_status = P_RUN;
		else if(can_dodge && key_held & (KEY_UP | KEY_DOWN) && last_dpad_key != (KEY_LEFT | KEY_RIGHT))
		{
		dodging = true;
		vel_y = 0;
		can_dodge = false;
		can_run = false;
		Jump();
	    }
		else if(!can_run && key_held & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN) && !jumping)
		{
			primary_status = P_WALK;
		}

		if(jumping && dodging)
		{
		slowing_down = false;
		primary_status = P_DODGING;
		}
		else
		dodging = false;

	}

	// character movement.
	void moveCharacter()
	{
	
	// primary and secondary states are cached for comparison in future frames.
	sprite.past_anim_status = sprite.anim_status;
	past_primary_status = primary_status;
	past_secondary_status = secondary_status;

	mapToScreenPos();
	
	// manages animation states.
	if(secondary_status == S_NONE)
	{
	    switch(primary_status)
	    {
		    case P_IDLE:
		    sprite.anim_status = A_IDLE; break;
		    case P_WALK:
		    sprite.anim_status = A_WALK; break;
		    case P_RUN:
		    sprite.anim_status = A_RUN; break;
		    case P_DODGING:
		    sprite.anim_status = A_JUMP; break;
	    }
    }
	else
	{
		switch(secondary_status)
		{
			case S_JUMP:
		    sprite.anim_status = A_JUMP; break;
		}
	}

    
	doubleTap();

	// manages character velocity bases on its status.
	switch (primary_status)
	{
		case P_IDLE:

		vel_y = 0; //ya que en el juego original no hay slow down en el eje y :D
		can_move = false;
		break;

		case P_WALK:
		can_move = true;
		if(!jumping)
		{
		if(key_held & KEY_RIGHT && !(key_held & KEY_LEFT))
		vel_x = 2;
		else if (key_held & KEY_LEFT && !(key_held & KEY_RIGHT))
		vel_x = -2;
		}
		
		else if(jumping && vel_x == 0)
		{
			if(last_dpad_key & KEY_LEFT)
			vel_x = -2;
			else if (last_dpad_key & KEY_RIGHT)
			vel_x = 2;
		}

		if(key_held & KEY_DOWN && !(key_held & KEY_UP))
		vel_y = 2;
		else if (key_held & KEY_UP && !(key_held & KEY_DOWN))
		vel_y = -2;
		else 
		vel_y = 0; //ya que en el juego original no hay slow down en el eje y :D
		break;

		case P_RUN:

		can_move = true;
		if(!jumping)
		{
		if(key_held & KEY_RIGHT && !(key_held & KEY_LEFT))
		vel_x = 5;
		else if (key_held & KEY_LEFT && !(key_held & KEY_RIGHT))
		vel_x = -5;
		}
		else if(jumping && vel_x == 0)
		{
			if(last_dpad_key & KEY_LEFT)
			vel_x = -5;
			else if (last_dpad_key & KEY_RIGHT)
			vel_x = 5;
		}

		if(key_held & KEY_DOWN && !(key_held & KEY_UP))
		vel_y = 2;
		else if (key_held & KEY_UP && !(key_held & KEY_DOWN))
		vel_y = -2;
		else
		vel_y = 0; //ya que en el juego original no hay slow down en el eje y :D
		break;

		case P_DODGING:

		if(vel_y == 0)
		{
		if(last_dpad_key & KEY_UP)
		vel_y = -2;
		else if (last_dpad_key & KEY_DOWN)
		vel_y = 2;
		}
		break;
	}

	// secondary status logic.
	switch (secondary_status)
	{
		case S_JUMP:
		Jump();
		break;
	}

	
	// key inputs for movement.
	// left.
	if(key_held & KEY_LEFT && !(key_held & KEY_RIGHT)){
		sprite.flipped = true;
	}
	// right.
	if(key_held & KEY_RIGHT && !(key_held & KEY_LEFT)){
		sprite.flipped = false;
	}

	if(!dodging)
	{
	// down.
	if(key_held & KEY_DOWN && !(key_held & KEY_UP)){
		if(!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
		primary_status = P_WALK;
	}
	// up.
	if(key_held & KEY_UP && !(key_held & KEY_DOWN)){
		if(!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
		primary_status = P_WALK;
	}
    }
    
	// idle.
	if(((key_current == 0 || (key_held & KEY_LEFT && key_held & KEY_RIGHT) || (key_held & KEY_UP && key_held & KEY_DOWN))) && !jumping)
	    {
		primary_status = P_IDLE;
	    can_move = false;
	    }

    // jump.
	if(key_pressed & KEY_A || jumping)
		secondary_status = S_JUMP;
	else
		secondary_status = S_NONE;



	// slowdown when character stops moving.

	// horizontal.
	if(((!(key_held & (KEY_LEFT | KEY_RIGHT))) || (key_held & KEY_LEFT && key_held & KEY_RIGHT)) && !jumping)
	{

	slowing_down = true;

	if (vel_x > 0) 
	acc_x = -0.2;
	else if (vel_x < 0) 
	acc_x = 0.2;
	else 
	acc_x = 0;
	}

	/*INNECESARIO, EN EL JUEGO ORIGINAL NO HAY DESACELERACION EN EL EJE Y :D
	//Vertical. REPENSAR CONDICION DE ACA, NO SIRVE, BUGEA, CACA
	if((primary_status_changed) && !(key_held & (KEY_UP | KEY_DOWN)) || (key_held & KEY_UP && key_held & KEY_DOWN))
	{

	slowing_down = true;

	if (vel_y > 0) 
	acc_y = -0.5;
	else if (vel_y < 0) 
	acc_y = 0.5;
	else 
	acc_y = 0;
	}
	*/

	if(std::abs(vel_x) <= 0.5f)
	{
	slowing_down = false;
	vel_x = 0;
	acc_x = 0;
	}
	

    // character movement.
	map_pos_x += vel_x;
	map_pos_y += vel_y;
	vel_x += acc_x;
	vel_y += acc_y;

	// shadow movement.
	//igual actualizar esto cuando la posicion en el mapa no tenga nada que ver con la posicion en pantalla
	//(por el scrolleo :P)
	shadow.screen_pos_x = map_pos_x;
	shadow.screen_pos_y = map_pos_y + 5;
	
    
	// did the states change?
	sprite.anim_status_changed = (sprite.past_anim_status != sprite.anim_status);
	primary_status_changed = (past_primary_status != primary_status);
	secondary_status_changed = (past_secondary_status != secondary_status);

	// mechanic to let the character move betweens screens.
	if(second_screen_enabled)
	{
	if (map_pos_y >= CHAR_SCREEN_SIZE || sprite.screen_pos_y <= -30)
	sprite.changeScreen();
	}
	}


	//Constructor.
	Character(int initial_pos_x, int initial_pos_y){
		map_pos_x = initial_pos_x;
		map_pos_y = initial_pos_y;
	}
};

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
    //Se inicializa el modo de graficos '0' en la pantalla de arriba.
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
	NF_LoadTiledBg("bg/bg1", "bg1", 256, 256);
	NF_CreateTiledBg(0, 3, "bg1");

	//Se carga la musica.
	NF_LoadRawSound("songs/song1", 0, 11025, 0);
	NF_PlayRawSound(0, 127, 64, true, 0);

	//Se crea al personaje "kim".
	Character kim(90, 76);
	//Se llenan sus atributos.
	//(luego creare un constructor que incluya toda la wea de abajo).
	kim.sprite.animated = true;
	kim.sprite.type = CHARACTER;
	kim.sprite.id = 0;
	kim.sprite.anim_status = A_IDLE;
	kim.primary_status = P_IDLE;
	kim.sprite.n_frames[A_IDLE] = 3;
	kim.sprite.n_frames[A_WALK] = 5;
	kim.sprite.n_frames[A_RUN] = 7;
	kim.sprite.n_frames[A_JUMP] = 12;
	kim.sprite.anim_frame_period[A_IDLE] = 8;
	kim.sprite.anim_frame_period[A_WALK] = 8;
	kim.sprite.anim_frame_period[A_RUN] = 4;
	kim.sprite.anim_frame_period[A_JUMP] = 6;
	kim.sprite.sprite_name = "kim";
	kim.sprite.screen = 0;
	//Se llaman a los metodos que alistan el sprite.
	kim.sprite.setupSprite();
	kim.setupShadow();
	kim.shadow.setupSprite();

	

	
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
		scanKeys();
		if(keysDown() & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT))
		last_dpad_key = keysDown();

		key_pressed = keysDown();
		key_held = keysHeld();
		key_current = keysCurrent();
		key_up = keysUp();

		//Metodos para actualizar el sprite cada frame.
		kim.moveCharacter();
		kim.sprite.updateSprite();
		kim.shadow.updateSprite();


		//CONSOLE DEBUG//
		
		consoleClear();
		
		std::cout << "\nSTART POS: " << kim.jump_start_pos
		          << "\nCURRENT SCREEN POS: " << kim.sprite.screen_pos_y
				  << "\nCURRENT MAP POS: " << kim.map_pos_y
				  << "\nSTATUS PRINCIPAL: " << kim.primary_status
				  << "\nSTATUS SECUNDARIO: " << kim.secondary_status
				  << "\nFRAMES SALTANDO: " << kim.frames_jumping
				  << "\nCAMBIO ESTADO PRIMARIO: " << kim.primary_status_changed
				  << "\nALENTANDOSE: " << kim.slowing_down
				  << "\nCAMBIO ANIMACION: " << kim.sprite.anim_status_changed
				  << "\nPUEDE MOVERSE: " << kim.can_move
				  << "\nPUEDE CORRER: " << kim.can_run
				  << "\nPUEDE ESQUIVAR: " << kim.can_dodge
				  << "\nESQUIVANDO: " << kim.dodging
				  << "\nSALTANDO: " << kim.jumping;
		
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

