#pragma once

#include "libs.h"
#include "enums.h"



// le constants.

constexpr uint8_t MAX_NUMBER_SPRITES = 20; //!!! ESTA WEA hace que se genere abajo varios arrays de tamaño lo que sea que valga esta wea, aunque no se ocupe. lo mejor seria hacer algun tipo de array dinamico.
constexpr uint8_t CHAR_SCREEN_SIZE = 190;
constexpr uint8_t NUM_EFFECTS = 2;


class Sprite
{
	public: //por ahora todos publicos pa testear, --luego hacerlos privados!! y hacer respectivos getters y setters--.
	bool animated = false;
	int num_sprites = 0;
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

    void assignSpriteDirs();
    void assignSpritesMemory();
    void freeSpritesMemory();
    void changeScreen();
    void createSprite();
	void deleteSprite();
    void animateSprite();
    void updateSpriteAndPalette();
    void updateSprite();
    void setupSprite();
	void setupSprite(CHARACTERS);
};