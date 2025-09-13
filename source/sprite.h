#pragma once

#include "libs.h"
#include "enums.h"



// le constants.

constexpr uint8_t MAX_NUMBER_SPRITES = 20; //!!! ESTA WEA hace que se genere abajo varios arrays de tamaño lo que sea que valga esta wea, aunque no se ocupe. lo mejor seria hacer algun tipo de array dinamico.
constexpr uint8_t CHAR_SCREEN_SIZE = 190;
constexpr uint8_t NUM_EFFECTS = 2;


// LEEME WEON!!!!!!!!!!!!!!!!1111111111111111111 //

// ahorita tengo 2 clases para sprites, una para sprites simples y otra para sprites mas complejos.
// pero la de sprites mas complejos se llama Sprite y la de sprites simples SingleSprite.
// la neta es una mrd y lo deberia de rehacer de cero de manera mas organizada pero ahorita
// me da weba, cuando quiera refactorizar lo hare :D

// IDEAS //
// MAYBE se podria hacer que la clase de aa bajito nomas sea para el sprite del personaje, llamarla
// CharacterSprite por ejemplo, y que lo que la distinga sea que puede tener varios sprites
// (pa los distintos estados del personaje, por ejemplo), pero que la otra sea para sprites que
// solo son una 'imagen' realmente.


class Sprite
{
	public: 
	bool animated = false;
	int num_sprites = 0;
	int gfx_id[MAX_NUMBER_SPRITES];
	int pal_id[MAX_NUMBER_SPRITES];
	int sprite_id[MAX_NUMBER_SPRITES];
	int gfx_vram_id[MAX_NUMBER_SPRITES];
	int pal_vram_id[MAX_NUMBER_SPRITES];
	int type = 0;
	int anim = 0;
	int anim_frame = 0;
	int anim_frame_period[MAX_NUMBER_SPRITES];
	int anim_direction = 1;
	int n_frames[MAX_NUMBER_SPRITES];
    int anim_status = 0;
	int past_anim_status = 0;
	bool anim_status_changed = false;
	std::string sprite_name = "";
	int scale_x = 256;
	int scale_y = 256;
	int scale_f = 0;
	int rot = 0;
	int slot_id = 0;
	std::string sprite_dir[MAX_NUMBER_SPRITES]; //Un espacio para cada sprite del personaje.
	std::string palette_dir[MAX_NUMBER_SPRITES];
	int screen = 0;
	int screen_pos_x = 0;
	int screen_pos_y = 0;
	int size_x = CHAR_DEFAULT_WIDTH;
	int size_y = CHAR_DEFAULT_LENGTH;
	bool flipped = false;
	bool looped = true;
	bool can_move = true;
	bool created = false;
	bool mirrored = false;
	bool rotscale = false;
	int n_of_sprites = 0;

    void assignSpriteDirs();
    void assignSpritesRAM();
	void assignSpritesVRAM();
    void freeSpritesRAM();
	void freeSpritesVRAM();
    void changeScreen();
    void createSprite();
	void deleteSprite();
	void createMirroredSprite();
    void animateSprite();
    void updateSpriteAndPalette();
    void updateSprite();
    void setupSprite();
	void setupSprite(CHARACTERS);
};

class SimpleSprite
{
	public:
	int screen = 0;
	int frame = 0;
	int size_x = 0;
	int size_y = 0;
	int pos_x = 0;
	int pos_y = 0;
	int gfx_ram_id = 0;
	int pal_ram_id = 0;
	int gfx_vram_id = 0;
	int pal_vram_id = 0;
	int *oam_id;

	std::string sprite_dir = "sprite/";
	std::string palette_dir = "palette/";
	std::string name = "";
	
	bool flipped = false;
	bool rotscale = false;
	bool made_of_many = false;
	bool custom_part_order = false;
	int num_of_parts;

	void assignSpriteRAM();
	void assignSpriteVRAM();
	void freeSpriteRAM();
	void freeSpriteVRAM();
	void createSprite(int, int);
	void deleteSprite();
	void setupSprite();


	SimpleSprite(int screen, int size_x, int size_y, std::string name, bool rotscale, int num_of_parts, bool custom_part_order);
};