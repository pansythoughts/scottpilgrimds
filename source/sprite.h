#include "libs.h"


// le constants.

constexpr uint8_t CHAR_NUM_SPRITES = 4; //CAMBIAR ESTO CADA QUE SE AGREGUE OTRO SPRITE!!!!!
constexpr uint8_t CHAR_SCREEN_SIZE = 190;

enum CHAR_SPRITE_SIZE {CHAR_DEFAULT_WIDTH = 64, CHAR_DEFAULT_LENGTH = 64};
enum CHAR_SPRITE_ANIMATION_STATES {A_IDLE = 0, A_WALK = 1, A_RUN = 2, A_JUMP = 3};
enum CHAR_SPRITE_PRIMARY_STATES {P_IDLE = 0, P_WALK = 1, P_RUN = 2, P_DODGING = 3};
enum CHAR_SPRITE_SECONDARY_STATES {S_NONE = 0, S_JUMP = 1};
enum SPRITE_TYPES {CHARACTER = 0, SHADOW = 1};

class Sprite
{
	public: //por ahora todos publicos pa testear, --luego hacerlos privados!! y hacer respectivos getters y setters--.
	bool animated = false;
	int type = 0;
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

    void assignSpriteDirs();
    void assignSpritesMemory();
    void freeSpritesMemory();
    void changeScreen();
    void createSprite();
    void animateSprite();
    void updateSpriteAndPalette();
    void updateSprite();
    void setupSprite();
};