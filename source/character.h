#pragma once

#include "libs.h"
#include "input.h"
#include "sprite.h"


class Character
{
	public:
	CHARACTERS char_id;
	std::string char_name = "";
	int walk_speed = 0;
	int run_speed = 0;
	int dodge_speed = 0;
	int num_sprites = 0;

    int frames = 0;
	int map_pos_x = 0;
	int map_pos_y = 0;
	int last_idle_pos_x = 0;
	int last_idle_pos_y = 0;
	int start_run_pos_x = 0;
	int start_walk_pos_x = 0;
	int start_walk_pos_y = 0;
	int start_jump_vel_x = 0;
	int distance_walked_x = 0;
	int distance_ran_x = 0;
	int distance_moved_y = 0;
	float vel_x = 0;
	float vel_y = 0;
	float jump_vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;
	float jump_acc_y = 0;
	int jump_height = 0;
	int primary_status = 0;
	int secondary_status = 0;
	int past_primary_status = 0;
	int past_secondary_status = 0;
	bool primary_status_changed = false;
	bool secondary_status_changed = false;
	bool can_move = false;
	bool slowing_down = false;
	int frames_moving = 0;
	int frames_idle = 0;
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

	Sprite effects[NUM_EFFECTS];

    // le methods.

	void setupCharacter();
    void setupSecondarySprites();
    void mapToScreenPos(int, int);
    void Jump();
    void doubleTap();
    void moveCharacter(int, int);
	void updateCharacter(int, int);
	void mapCollision(LEVELS current_lvl);
    Character(CHARACTERS);
};