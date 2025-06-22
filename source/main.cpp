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
enum CHAR_SPRITE_ANIMATION_STATES {IDLE = 0, WALK = 1, RUN = 2, JUMP = 3};


//Variables globales.//

bool nfInit;

//Clases//

class Sprite;
class Character;

class Sprite{
	//Atributos.
	public: //por ahora todos publicos pa testear, --luego hacerlos privados!! y hacer respectivos getters y setters--.
	bool animated = false;
	int anim = 0;
	int anim_frame = 0;
	int anim_frame_speed[CHAR_NUM_SPRITES];
	int anim_direction = 1;
	int n_frames[CHAR_NUM_SPRITES];
    int anim_status = 0;
	int past_anim_status = 0;
	bool anim_status_changed = false;
	std::string sprite_name = ""; //CAMBIO HECHO: empece a usar 'strings' de verdad de la std, en vez de 'char[]' como en C.
	int scale = 0;
	int scale_f = 0;
	int rot = 0;
	int id = 0;
	int ram_slot = 0;
	std::string sprite_dir[CHAR_NUM_SPRITES]; //Un espacio para cada sprite del personaje.
	std::string palette_dir[CHAR_NUM_SPRITES];
	int screen = 0;
	int pos_x = 0;
	int pos_y = 0;
	int size_x = CHAR_DEFAULT_WIDTH;
	int size_y = CHAR_DEFAULT_LENGTH;
	bool flipped = false;

	//Metodos//

	//Asigna los directorios de cada sprite del personaje.
    void assignSpriteDirs(){
	for(int i = 0; i < CHAR_NUM_SPRITES; i++){
	sprite_dir[i] = "sprite/" + sprite_name;	
	palette_dir[i] = "palette/" + sprite_name;
	switch(i){
		case IDLE:
		sprite_dir[i] += "_idl";
		palette_dir[i] += "_idl"; break;
		case WALK:
		sprite_dir[i] += "_walk";
		palette_dir[i] += "_walk"; break;
		case RUN:
		sprite_dir[i] += "_run";
		palette_dir[i] += "_run"; break;
		case JUMP:
		sprite_dir[i] += "_jump";
		palette_dir[i] += "_jump"; break;
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
	    if (pos_y >= CHAR_SCREEN_SIZE)
			pos_y = CHAR_SCREEN_SIZE - pos_y;
		else if (pos_y <= -30)
		    pos_y = CHAR_SCREEN_SIZE + pos_y;
			
			NF_DeleteSprite(screen, id);
		    freeSpritesMemory();
		    if (screen == 0) screen = 1;
		    else screen = 0;
		    assignSpritesMemory();
		    NF_CreateSprite(screen, id, id, id, pos_x, pos_y);
	}

    //Crea un sprite.
    void createSprite(){
    //Mostrando el sprite en pantalla.
	NF_CreateSprite(screen, id, id, id, pos_x, pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
	NF_EnableSpriteRotScale(screen, id, id, false);//(screen, sprite ID, rot ID, 'doublesize') <--- por algun motivo desplaza el sprite un poco abajo a la derecha.
	}

	//Anima un sprite (si es animado).
    void animateSprite(){
		if (animated){
		//Animacion del sprite.
		anim += anim_direction;
		if(anim >= anim_frame_speed[anim_status]){
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
	NF_CreateSprite(screen, id, id, id, pos_x, pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
}
    //Actualiza al sprite (posicion, tipo de sprite y paleta.).
    void updateSprite(){
	    //Se guarda el estado de la animacion del frame, para compararlo en futuros frames.
	    past_anim_status = anim_status;
		if(anim_status >= 0 && anim_status <= CHAR_NUM_SPRITES){
	    //Se actualiza la posicion del sprite.
		if(anim_status_changed)
		changeSpriteAndPalette();
		NF_MoveSprite(screen, id, pos_x, pos_y);//(screen, id, pos x, pos y)
		if(flipped)
		NF_HflipSprite(screen, id, true);
}
}
};

class Character{
	//Atributos. (lo mismo, luego los hago privados :D)
	public:
	char name[10];
	float vel_x;
	float vel_y;
	float acc_x = 0;
	float acc_y = 0;
	int status; //0.IDLE,1.WALK,2.RUN.
	int frames_moving = 0;
	int frames_jumping = 0;
	bool jumping = false;
	int jump_start_pos = 0;
	int jump_final_pos = 0;
	bool canDTap = false;
	bool canRun = false;
	int lastTapTime = 0;    // Guardará el tiempo del último toque
    int doubleTapThreshold = 15; // Cuántos frames para considerar un doble toque
	Sprite sprite;

	//Metodos//

	//Maneja el 'double tap' para correr. 
	//(luego hacerlo para saltar hacia arriba y abajo).
	void doubleTap(){
		// Revisa si el jugador está presionando las teclas de movimiento
		if (keysHeld() == KEY_LEFT || keysHeld() == KEY_RIGHT) {
			frames_moving++;  // Incrementa los frames de movimiento
		
		canRun = canDTap && (frames_moving - lastTapTime <= doubleTapThreshold);
		// Si se detecta un segundo toque dentro del tiempo límite, activa el correr
		if (canRun)
			status = RUN;
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
			status = WALK;
	}

	void moveCharacter(){
	//Se pasa el valor de 'status' del objeto 'Character', al objeto 'Sprite' dentro de el.
	sprite.anim_status = status;
    //Maneja todo lo relacionado al doble tap para correr.
	doubleTap();
	//Se maneja la velocidad del personaje segun sus estados.
	switch (status){
		//Quieto (idle).
		case IDLE:
		vel_x = 0;
		vel_y = 0; break;
		//Caminando (walking).
		case WALK:
		vel_x = 2;
		vel_y = 2; break;
		//Corriendo (running).
		case RUN:
		vel_x = 5;
		vel_y = 5; break;

		case JUMP:
		if(!jumping)
		{
		sprite.anim_frame = 0;
		jumping = true;
		jump_start_pos = sprite.pos_y;
		vel_y = -6.7;
		}
		
		if(jumping)
		frames_jumping++;

		if(frames_jumping >= 1 && jumping)
		{
		acc_y = 0.3;
		vel_y += acc_y;
		sprite.pos_y += vel_y;
	    }

		if(vel_y <= 0 && vel_y >= -0.5)
		{
			if(sprite.anim_frame == 1 || sprite.anim_frame == 3)
			sprite.anim_direction *= -1;
		}
		else
		{
			sprite.anim_direction = 1;
		}

	    if(sprite.pos_y >= jump_start_pos)
		{
		sprite.anim_frame = 12;
		jump_final_pos = sprite.pos_y;
		sprite.pos_y -= (jump_final_pos - jump_start_pos);
		frames_jumping = 0;
		jumping = false;
		}
		break;
	
	}
	//Entradas de teclas del usuario para mover al personaje.
	if(keysHeld() == KEY_LEFT){
		sprite.pos_x -= vel_x;
		NF_HflipSprite(0, 0, true);
		NF_HflipSprite(0, sprite.id, true);
		sprite.flipped = true;
	}
	if(keysHeld() == KEY_RIGHT){
		sprite.pos_x += vel_x;
		NF_HflipSprite(0, 0, false);
		NF_HflipSprite(0, sprite.id, false);
		sprite.flipped = false;
	}
	if(keysHeld() == KEY_DOWN){
		status = WALK;
		sprite.pos_y += vel_y;
	}
	if(keysHeld() == KEY_UP){
		status = WALK;
		sprite.pos_y -= vel_y;
	}
	if(keysHeld() == 0){
		status = IDLE;
	}
	if(keysCurrent() == KEY_A || jumping){
		status = JUMP;
	}
    
	//Determina si hubo un cambio en el estado o 'status'.
	sprite.anim_status_changed = (sprite.past_anim_status != sprite.anim_status);

	//DESCOMENTAR PARA PERMITIR CAMBIAR PANTALLA//
	/*
	if (sprite.pos_y >= CHAR_SCREEN_SIZE || sprite.pos_y <= -30)
	 sprite.changeScreen();
	*/
	}
	//Constructor
	Character(int initial_pos_x, int initial_pos_y){
		sprite.pos_x = initial_pos_x;
		sprite.pos_y = initial_pos_y;
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
	}
    //Se inicializa el modo de graficos '0' en la pantalla de arriba.
	NF_Set2D(0, 0);//(screen, mode).

	//DESCOMENTAR PARA PERMITIR CAMBIAR PANTALLA.//
	//NF_Set2D(1, 0);
	

    //Se prepara todo lo relacionado al sistema de sprites.
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0); //'0' se refiere a la pantalla, en este caso es la superior.

	//DESCOMENTAR PARA PERMITIR CAMBIAR PANTALLA.//
	//NF_InitSpriteSys(1);

	//Se crea al personaje "kim".
	Character kim(50, 50);
	//Se llenan sus atributos.
	//(luego creare un constructor que incluya toda la wea de abajo).
	kim.sprite.animated = true;
	kim.sprite.id = 0;
	kim.sprite.anim_status = IDLE;
	kim.status = IDLE;
	kim.sprite.ram_slot = 0;
	kim.sprite.n_frames[IDLE] = 3;
	kim.sprite.n_frames[WALK] = 5;
	kim.sprite.n_frames[RUN] = 7;
	kim.sprite.n_frames[JUMP] = 12;
	kim.sprite.anim_frame_speed[IDLE] = 8;
	kim.sprite.anim_frame_speed[WALK] = 8;
	kim.sprite.anim_frame_speed[RUN] = 5;
	kim.sprite.anim_frame_speed[JUMP] = 6;
	kim.sprite.sprite_name = "kim";
	kim.sprite.screen = 0;
	//Se llaman a los metodos que alistan el sprite.
	kim.sprite.assignSpriteDirs();
	kim.sprite.assignSpritesMemory();
	kim.sprite.createSprite();
	

	
	//texto pitero :P
	std::cout << "LO QUE ME COSTO ESTE PEDAZO DE \nMIERDA NOMAMES CHINGUEN TODOS A \nSU MADRE\n\n";


	//Bucle principal
	while (nfInit) 
	{

		//OJO: NO LLAMAR ACA A NF_CreateSprite(), PUES HARA QUE LA ANIMACION SE ENTRECORTE.
		//LLAMARLA UNA SOLA VEZ ANTES DEL LOOP.



		//Se registran los inputs del usuario.
		scanKeys();

		//Metodos para actualizar el sprite cada frame.
		if(kim.sprite.anim_status_changed){
		kim.sprite.updateSprite();
		kim.sprite.freeSpritesMemory();
		kim.sprite.assignSpritesMemory();
		}
		kim.sprite.updateSprite();
		kim.sprite.animateSprite();
		kim.moveCharacter();


		//CONSOLE DEBUG//
		if(keysCurrent() == KEY_B)
		{
		consoleClear();
		std::cout << "Vram libre: " << NF_TEXVRAM.free;
		}
		if(kim.sprite.anim_status_changed)
		std::cout << "\nCAMBIO";

		if(kim.jumping || keysCurrent() == KEY_A)
		{
		consoleClear();
		std::cout << "FRAMES SALTANDO: " << kim.frames_jumping;
		}
		
		consoleClear();
		if(kim.sprite.anim_status_changed)
		std::cout << "\n\nCAMBIO";
		
		std::cout << "\nSTART POS: " << kim.jump_start_pos
		          << "\nCURRENT POS: " << kim.sprite.pos_y
				  << "\nSTATUS: " << kim.status;
		
		//DESCOMENTAR PARA PERMITIR CAMBIAR PANTALLA.//
		/*
		if(keysDown() == KEY_X)
		kim.sprite.changeScreen();
		*/

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

