//Librerias//
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <nf_lib.h>
#include <string>
#include <iostream>
#include <gl2d.h>
#include <filesystem.h>

//Definiciones de constantes//

constexpr uint8_t CHAR_NUM_SPRITES = 4; //CAMBIAR ESTO CADA QUE SE AGREGUE OTRO SPRITE!!!!!
constexpr uint8_t CHAR_SCREEN_SIZE = 190;

enum CHAR_SPRITE_SIZE {CHAR_DEFAULT_WIDTH = 64, CHAR_DEFAULT_LENGTH = 64};
enum CHAR_SPRITE_ANIMATION_STATES {A_IDLE = 0, A_WALK = 1, A_RUN = 2, A_JUMP = 3};
enum CHAR_SPRITE_PRIMARY_STATES {P_IDLE = 0, P_WALK = 1, P_RUN = 2, P_DODGING = 3};
enum CHAR_SPRITE_SECONDARY_STATES {S_NONE = 0, S_JUMP = 1};



//Variables globales.//

bool nfInit;
bool second_screen_enabled = false; //Cambiar esto para permitir todo lo de cambiar pantallas.
int key_pressed = 0;
int key_held = 0;
int key_current = 0;
int key_down_repeat = 0;
int key_up = 0;
int last_dpad_key = 0;
int frames = 0;

//Clases//

class Sprite;
class Character;

class Sprite{
	//Atributos.
	public: //por ahora todos publicos pa testear, --luego hacerlos privados!! y hacer respectivos getters y setters--.
	bool animated = false;
	bool is_shadow = false;
	int anim = 0;
	int anim_frame = 0;
	int anim_frame_period[CHAR_NUM_SPRITES];
	int anim_direction = 1;
	int n_frames[CHAR_NUM_SPRITES];
    int anim_status = 0;
	int past_anim_status = 0;
	bool anim_status_changed = false;
	std::string sprite_name = ""; //CAMBIO HECHO: empece a usar 'strings' de verdad de la std, en vez de 'char[]' como en C.
	int scale_x = 256;
	int scale_y = 256;
	int scale_f = 0;
	int rot = 0;
	int id = 0;
	std::string sprite_dir[CHAR_NUM_SPRITES]; //Un espacio para cada sprite del personaje.
	std::string palette_dir[CHAR_NUM_SPRITES];
	int screen = 0;
	int screen_pos_x = 0;
	int screen_pos_y = 0;
	int size_x = CHAR_DEFAULT_WIDTH;
	int size_y = CHAR_DEFAULT_LENGTH;
	bool flipped = false;

	//Metodos//

	//Asigna los directorios de cada sprite del personaje.
    void assignSpriteDirs(){
	// ya que se use esta misma clase para varios tipos de sprites (personajes, objetos, etc.)
	// se podria usar un switch para asignar los directorios de manera unica a cada tipo
	// en vez del if de abajo.
	if(is_shadow)
	{
	sprite_dir[0] = "sprite/shadow";
	palette_dir[0] = "palette/shadow";
	}
	else
	{
	for(int i = 0; i < CHAR_NUM_SPRITES; i++){
	sprite_dir[i] = "sprite/" + sprite_name;	
	palette_dir[i] = "palette/" + sprite_name;
	switch(i){
		case A_IDLE:
		sprite_dir[i] += "_idl";
		palette_dir[i] += "_idl"; break;
		case A_WALK:
		sprite_dir[i] += "_walk";
		palette_dir[i] += "_walk"; break;
		case A_RUN:
		sprite_dir[i] += "_run";
		palette_dir[i] += "_run"; break;
		case A_JUMP:
		sprite_dir[i] += "_jump";
		palette_dir[i] += "_jump"; break;
	}
	}
    }
}
    //Manda los sprites de un personaje a la RAM, y luego a la VRAM.
	//OJO: posible causante del error 'Data abort.' cuando se ejecuta en hardware real.
	//(puede que haya overflow de memoria(?))
	//^
	//|
	//MENTIROTA ALABERGA. SE CAUSABA POR VARIABLES NO INICIALIZADAS.
    void assignSpritesMemory(){
	NF_LoadSpriteGfx(sprite_dir[anim_status].c_str(), id, size_x, size_y);//(file, RAM slot, size x, size y) 
	NF_LoadSpritePal(palette_dir[anim_status].c_str(), id);//(file, RAM slot)(c_str() para convertir de std::string a char[])

    //Cargar el sprite junto a su paleta de la RAM a la VRAM.
	NF_VramSpriteGfx(screen, id, id, true);//(screen, RAM slot, VRAM slot, copy all frames to VRAM)
	                                                                //OJO: si se quiere mostrar un sprite animado, y se ocupa que se cree
																	//(llamar a NF_CreateSprite()) repetidamente en el bucle, debera ser 'true',
																	//Si se llama a una vez, de ambas maneras mostrara correctamente la animacion.
	NF_VramSpritePal(screen, id, id);      //(screen, RAM slot, VRAM slot)
	}

	//Libera el espacio de memoria utilizado por el sprite de un personaje.
	void freeSpritesMemory(){
	NF_FreeSpriteGfx(screen, id);
	NF_UnloadSpriteGfx(id);
	NF_UnloadSpritePal(id);

	}

	//Cambia un sprite de pantalla, con la misma posicion relativa con la que se quedo.
	void changeScreen(){
	    if (screen_pos_y >= CHAR_SCREEN_SIZE)
			screen_pos_y = CHAR_SCREEN_SIZE - screen_pos_y;
		else if (screen_pos_y <= -30)
		    screen_pos_y = CHAR_SCREEN_SIZE + screen_pos_y;
			
			NF_DeleteSprite(screen, id);
		    freeSpritesMemory();
		    if (screen == 0) screen = 1;
		    else screen = 0;
		    assignSpritesMemory();
		    NF_CreateSprite(screen, id, id, id, screen_pos_x, screen_pos_y);
	}

    //Crea un sprite.
    void createSprite(){
    //Mostrando el sprite en pantalla.
	NF_CreateSprite(screen, id, id, id, screen_pos_x, screen_pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
	NF_EnableSpriteRotScale(screen, id, id, false);//(screen, sprite ID, rot ID, 'doublesize') <--- por algun motivo desplaza el sprite un poco abajo a la derecha.
	}

	//Anima un sprite (si es animado).
    void animateSprite(){
		if (animated){
		//Animacion del sprite.
		anim += anim_direction;
		if(anim >= anim_frame_period[anim_status]){
			anim = 0;
			anim_frame++;
			if(anim_frame > n_frames[anim_status]){
				anim_frame = 0;
			}
			NF_SpriteFrame(screen, id, anim_frame); //screen, id, frame.
		}
	}
}
    //Cambia el tipo de sprite y paleta del mismo personaje.
    void changeSpriteAndPalette(){
	NF_DeleteSprite(screen, id);
	NF_CreateSprite(screen, id, id, id, screen_pos_x, screen_pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
}
    //Actualiza al sprite (posicion, tipo de sprite y paleta.).
    void updateSprite(){

	    //Se guarda el estado de la animacion del frame, para compararlo en futuros frames.
	    past_anim_status = anim_status;

		if(anim_status >= 0 && anim_status <= CHAR_NUM_SPRITES){

	    //Se actualiza la posicion del sprite.
		if(anim_status_changed)
		changeSpriteAndPalette();

		NF_MoveSprite(screen, id, screen_pos_x, screen_pos_y);
		NF_SpriteRotScale(screen, id, rot, scale_x, scale_y);

		if(flipped)
		NF_HflipSprite(screen, id, true);
		else
		NF_HflipSprite(screen, id, false);
}
}

void setupSprite(){

	assignSpriteDirs();
	assignSpritesMemory();
	createSprite();
}
};

class Character{
	//Atributos. (lo mismo, luego los hago privados :D)
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

	//Metodos//

    void setupShadow(){
		shadow.sprite_name = sprite.sprite_name + "_shadow";
		shadow.is_shadow = true;
		shadow.id = sprite.id + 1;
	}

	//Genera la posicion en pantalla del personaje.
	void mapToScreenPos(){
		//luego hare una version mas aca que permita el scrolleo :D
		sprite.screen_pos_x = map_pos_x;
		sprite.screen_pos_y = map_pos_y + jump_height;
	
		

	}

	//El personaje realiza la accion de saltar.
	void Jump(){

		if(!jumping)
		{
		sprite.anim_frame = 0;
		vel_x = 0;
		jumping = true;

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
		jump_acc_y = 0.3;
		jump_vel_y += jump_acc_y;
		jump_height += jump_vel_y;
	    }

		if(jump_vel_y <= 0 && jump_vel_y >= -0.5 && !dodging)
		{
			if(sprite.anim_frame == 1 || sprite.anim_frame == 3)
			sprite.anim_direction *= -1;
		}
		else
			sprite.anim_direction = 1;

		if(jump_height <= 2 && jump_height >= 6 && frames_jumping > 20)
		sprite.anim_frame = 12;

	    if(jump_height >= 0)
		{
		sprite.anim_frame = 12;
		frames_jumping = 0;
		jumping = false;
		}
		
		if(frames_jumping % 10 == 0)
		{
		shadow.scale_x = 256 + (2.5f * jump_height);
		shadow.scale_y = 256 + (2.5f * jump_height);
		}
	}
	

	//Maneja el 'double tap' para correr. 
	//(luego hacerlo para saltar hacia arriba y abajo).
	void doubleTap(){

		/*
		if(key_held & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN))
		{
		frames_moving++;
		}


		if(key_up & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN) && frames_moving <= double_tap_threshold)
		{
			can_run_or_dodge = can_dtap && (frames_moving - last_tap_time <= double_tap_threshold);
			can_dtap = true;
			last_tap_time = frames_moving;
		}

		if (frames_moving - last_tap_time > double_tap_threshold || jumping) {
				can_dtap = false;
		}
		*/

	
		if(key_pressed & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN))
		{
		//Horizontal (correr).
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

		//Vertical (esquivar).
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





		/*
		// Revisa si el jugador está presionando las teclas de movimiento
		if (keysHeld() & KEY_LEFT || keysHeld() & KEY_RIGHT) {
			frames_moving++;  // Incrementa los frames de movimiento
		
		canRun = canDTap && (frames_moving - lastTapTime <= doubleTapThreshold);
		// Si se detecta un segundo toque dentro del tiempo límite, activa el correr
		if (canRun)
			primary_status = P_RUN;
		}
		// Si se ha soltado la tecla, intentamos registrar el primer toque
		 if (keysUp() == KEY_LEFT || keysUp() == KEY_RIGHT) {
			if (frames_moving <= doubleTapThreshold) {
				canDTap = true; // Primer toque registrado
				lastTapTime = frames_moving; // Guarda el tiempo del primer toque
			}
			// Resetea la variable canDTap después de un cierto tiempo si no se detecta el segundo toque
			if (frames_moving - lastTapTime > doubleTapThreshold) {
				canDTap = false;
				frames_moving = 0; // Reinicia el contador de frames después de un tiempo de espera
			}
		}
		//Si no puede harcer 'double tap' o esta yendo hacia arriba o abajo, camina.
		if(!canDTap && ((keysHeld() == KEY_UP) || (keysHeld() == KEY_DOWN) || (keysHeld() == KEY_LEFT) ||(keysHeld() == KEY_RIGHT)))
			primary_status = P_WALK;
		*/
	}

	//Maneja lo relacionado al movimiento del personaje.
	void moveCharacter(){
	
	//Se guardan los estados del personaje para compararlo en futuros frames.
	past_primary_status = primary_status;
	past_secondary_status = secondary_status;

	mapToScreenPos();
	
	//Se maneja el estado de animacion del sprite.
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

    //Maneja todo lo relacionado al doble tap para correr.
	doubleTap();

	//Se maneja la velocidad del personaje segun sus estados primarios.
	switch (primary_status)
	{
		//Quieto (idle).
		case P_IDLE:
		vel_y = 0; //ya que en el juego original no hay slow down en el eje y :D
		can_move = false;
		break;
		//Caminando (walking).
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
		//Corriendo (running).
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
		//Esquivando.
		case P_DODGING:
		if(vel_y == 0)
		{
		if(last_dpad_key & KEY_UP)
		vel_y = -1.5;
		else if (last_dpad_key & KEY_DOWN)
		vel_y = 1.5;
		}
		break;
	}

	//Se maneja la logica de los estados secundarios.
	switch (secondary_status)
	{
		//Salto del personaje.
		case S_JUMP:
		Jump();

		break;
	}

	
	//Entradas de teclas del usuario para mover al personaje.
	//Izquierda.
	if(key_held & KEY_LEFT && !(key_held & KEY_RIGHT)){
		sprite.flipped = true;
	}
	//Derecha.
	if(key_held & KEY_RIGHT && !(key_held & KEY_LEFT)){
		sprite.flipped = false;
	}

	if(!dodging)
	{
	//Abajo.
	if(key_held & KEY_DOWN && !(key_held & KEY_UP)){
		if(!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
		primary_status = P_WALK;
	}
	//Arriba.
	if(key_held & KEY_UP && !(key_held & KEY_DOWN)){
		if(!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
		primary_status = P_WALK;
	}
    }
    
	//Quieto.
	if(((key_current == 0 || (key_held & KEY_LEFT && key_held & KEY_RIGHT) || (key_held & KEY_UP && key_held & KEY_DOWN))) && !jumping)
	    {
		primary_status = P_IDLE;
	    can_move = false;
	    }

    //Salto.
	if(key_pressed & KEY_A || jumping)
		secondary_status = S_JUMP;
	else
		secondary_status = S_NONE;



	//Desaceleracion cuando el personaje deja de moverse.

	//Horizontal.
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
	

    //Movimiento del personaje.
	map_pos_x += vel_x;
	map_pos_y += vel_y;
	vel_x += acc_x;
	vel_y += acc_y;

	//Movimiento de la sombra.
	shadow.screen_pos_x = map_pos_x;
	shadow.screen_pos_y = map_pos_y + 5;
	
    
	//Determina si hubo un cambio en el estado o 'status'.
	sprite.anim_status_changed = (sprite.past_anim_status != sprite.anim_status);
	primary_status_changed = (past_primary_status != primary_status);
	secondary_status_changed = (past_secondary_status != secondary_status);

	//Mecanica que permite al personaje moverse entre pantallas.
	if(second_screen_enabled)
	{
	if (map_pos_y >= CHAR_SCREEN_SIZE || sprite.screen_pos_y <= -30)
	sprite.changeScreen();
	}
	}


	//Constructor
	Character(int initial_pos_x, int initial_pos_y){
		map_pos_x = initial_pos_x;
		map_pos_y = initial_pos_y;
	}
};

//Prototipos de funciones.//

//Funcion principal.
int main() {

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
	Character kim(50, 50);
	//Se llenan sus atributos.
	//(luego creare un constructor que incluya toda la wea de abajo).
	kim.sprite.animated = true;
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
		if(kim.sprite.anim_status_changed){
		kim.sprite.updateSprite();
		kim.sprite.freeSpritesMemory();
		kim.sprite.assignSpritesMemory();
		}
		kim.sprite.updateSprite();
		kim.sprite.animateSprite();
		kim.shadow.updateSprite();
		kim.moveCharacter();


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

