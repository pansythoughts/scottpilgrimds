#include "libs.h"
#include "sprite.h"
#include "input.h"
#include "character.h"

// character set-up. goes in the constructor.
void Character::setupCharacter()
{
	switch (char_id)
	{
	case KIM:
		char_name = "kim";
		sprite.sprite_name = "kim";
		sprite.num_sprites = 4;
		sprite.animated = true;
		sprite.looped = true;
		sprite.screen = 0;
		sprite.type = SPR_CHARACTER;
		sprite.n_frames[A_IDLE] = 3;
		sprite.n_frames[A_WALK] = 5;
		sprite.n_frames[A_RUN] = 7;
		sprite.n_frames[A_JUMP] = 12;
		sprite.anim_frame_period[A_IDLE] = 8;
		sprite.anim_frame_period[A_WALK] = 8;
		sprite.anim_frame_period[A_RUN] = 4;
		sprite.anim_frame_period[A_JUMP] = 6;

		walk_speed = 2;
		run_speed = 5;
		dodge_speed = 2;
		break;
	}
}

void Character::setupSecondarySprites()
{
	// sets up character's shadow.
	shadow.type = SPR_SHADOW;
	shadow.sprite_name = sprite.sprite_name + "_shadow";
	shadow.setupSprite();

	// sets up character's effects.
	effects[EFF_RUN_EFFECT].type = SPR_RUN_EFFECT;
	effects[EFF_RUN_EFFECT].sprite_name = sprite.sprite_name + "_run_effect";
	effects[EFF_RUN_EFFECT].setupSprite();

	effects[EFF_LAND_EFFECT].type = SPR_LAND_EFFECT;
	effects[EFF_LAND_EFFECT].sprite_name = sprite.sprite_name + "_land_effect";
	effects[EFF_LAND_EFFECT].setupSprite();
}

// calculates the current screen position of the character.
void Character::mapToScreenPos(int map_scroll_x, int map_scroll_y)
{
	sprite.screen_pos_x = map_pos_x - map_scroll_x;
	sprite.screen_pos_y = map_pos_y + jump_height - map_scroll_y;
}

// le jump wooo.
void Character::Jump()
{

	if (!jumping)
	{
		start_jump_vel_x = vel_x;
		sprite.anim_frame = 0;
		vel_x = 0;
		jumping = true;

		// applies initial velocity.
		if (!dodging)
			jump_vel_y = -6.7;
		else
			jump_vel_y = -4;
	}

	if (jumping)
		frames_jumping++;

	if (frames_jumping >= 1 && jumping)
	{
		// jump physics.
		jump_acc_y = 0.3;
		jump_vel_y += jump_acc_y;
		jump_height += jump_vel_y;
	}

	// controls animation frames (so that the animation happens as in the original game :p).
	if (jump_vel_y <= 0 && jump_vel_y >= -0.5 && !dodging)
	{
		if (sprite.anim_frame == 1 || sprite.anim_frame == 3)
			sprite.anim_direction *= -1;
	}
	else
		sprite.anim_direction = 1;

	// same as before.
	if (jump_height <= 2 && jump_height >= 6 && frames_jumping > 20)
		sprite.anim_frame = 12;

	if (jump_height >= 0)
	{
		// stops jumping.
		sprite.anim_frame = 12;
		frames_jumping = 0;
		vel_x = 0;
		jumping = false;
	}
}

// manages 'double tap' mechanic. ALSO manages movement in general.
void Character::doubleTap()
{
	if (key_pressed & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN))
	{

		// horizontal (for running).
		if (key_pressed & (KEY_LEFT | KEY_RIGHT))
		{
			if (frames - last_tap_time_x <= double_tap_threshold && !jumping && frames > double_tap_threshold)
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
		else if (key_pressed & (KEY_UP | KEY_DOWN))
		{
			if (frames - last_tap_time_y <= double_tap_threshold && !jumping && frames > double_tap_threshold)
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

	if (frames - last_tap_time_x > double_tap_threshold && frames - last_tap_time_y > double_tap_threshold)
		can_dtap = false;

	// manages the different states based on the double tap info.
	if (can_run && key_held & (KEY_LEFT | KEY_RIGHT) && last_dpad_key != (KEY_UP | KEY_DOWN))
		primary_status = P_RUN;
	else if (can_dodge && key_held & (KEY_UP | KEY_DOWN) && last_dpad_key != (KEY_LEFT | KEY_RIGHT))
	{
		// start dodging.
		dodging = true;
		vel_y = 0;
		can_dodge = false;
		can_run = false;
		Jump();
	}
	else if (!can_run && key_held & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN) && !jumping)
	{
		primary_status = P_WALK;
	}

	if (jumping && dodging)
	{
		slowing_down = false;
		primary_status = P_DODGING;
	}
	else
		dodging = false;
}

// character movement.
void Character::moveCharacter(int bg_scroll_x, int bg_scroll_y)
{

	// primary and secondary states are cached for comparison in future frames.
	sprite.past_anim_status = sprite.anim_status;
	past_primary_status = primary_status;
	past_secondary_status = secondary_status;

	// manages animation states.
	if (secondary_status == S_NONE)
	{

		// if was and stopped jumping, create landing effect.
		if (!effects[EFF_LAND_EFFECT].created && sprite.anim_status == A_JUMP && frames_jumping <= 1)
			effects[EFF_LAND_EFFECT].createMirroredSprite();

		switch (primary_status)
		{
		case P_IDLE:
			sprite.anim_status = A_IDLE;
			break;
		case P_WALK:
			sprite.anim_status = A_WALK;
			break;
		case P_RUN:
			sprite.anim_status = A_RUN;

			if (!effects[EFF_RUN_EFFECT].created && frames_moving <= 1)
				effects[EFF_RUN_EFFECT].createSprite();
			break;
		case P_DODGING:
			sprite.anim_status = A_JUMP;
			break;
		}
	}
	else
	{
		switch (secondary_status)
		{
		case S_JUMP:
			sprite.anim_status = A_JUMP;
			break;
		}
	}

	// le double tap.
	doubleTap();

	// manages character movement based on its status.
	switch (primary_status)
	{
	// IDLE //
	case P_IDLE:

		// update frame counters.
		frames_idle++;
		frames_moving = 0;

		// if it started jumping, keep the momentum.
		if (jumping && vel_x == 0)
			vel_x = start_jump_vel_x;

		vel_y = 0; // ya que en el juego original no hay slow down en el eje y :D
		can_move = false;

		// caches last idle position.
		if (past_primary_status == P_IDLE && vel_x == 0 && vel_y == 0)
		{
			last_idle_pos_x = map_pos_x;
			last_idle_pos_y = map_pos_y;
		}

		break;

	// WALKING //
	case P_WALK:

		// update frame counters.
		frames_moving++;
		frames_idle = 0;

		can_move = true;

		// cache the map position when it started walking.
		if (frames_moving <= 1)
		{
			start_walk_pos_x = map_pos_x;
			start_walk_pos_y = map_pos_y;
		}

		// horizontal movement.

		if (!jumping)
		{
			if (key_held & KEY_RIGHT && !(key_held & KEY_LEFT))
				vel_x = walk_speed;
			else if (key_held & KEY_LEFT && !(key_held & KEY_RIGHT))
				vel_x = -walk_speed;
		}
		// if it started jumping, keep the momentum.
		else if (jumping && vel_x == 0)
			vel_x = start_jump_vel_x;

		// vertical movement.

		if (key_held & KEY_DOWN && !(key_held & KEY_UP))
			vel_y = walk_speed;
		else if (key_held & KEY_UP && !(key_held & KEY_DOWN))
			vel_y = -walk_speed;
		else
			vel_y = 0; // no y-axis slowdown on original game :D

		break;

	// RUNNING //
	case P_RUN:

		// update frame counters.
		frames_moving++;
		frames_idle = 0;

		can_move = true;

		// cache the map position when it started running.
		if (frames_moving <= 1)
		{
			start_run_pos_x = map_pos_x;
			start_walk_pos_y = map_pos_y;
		}

		// horizontal movement.

		if (!jumping)
		{
			if (key_held & KEY_RIGHT && !(key_held & KEY_LEFT))
				vel_x = run_speed;
			else if (key_held & KEY_LEFT && !(key_held & KEY_RIGHT))
				vel_x = -run_speed;
		}
		// if it started jumping, keep the momentum.
		else if (jumping && vel_x == 0)
			vel_x = start_jump_vel_x;

		// vertical movement.

		if (key_held & KEY_DOWN && !(key_held & KEY_UP))
			vel_y = walk_speed;
		else if (key_held & KEY_UP && !(key_held & KEY_DOWN))
			vel_y = -walk_speed;
		else
			vel_y = 0; // no y-axis slowdown on original game :D

		break;

	// DODGING //
	case P_DODGING:

		// update frame counters.
		frames_moving++;
		frames_idle = 0;

		// if it started dodging...
		if (vel_y == 0)
		{
			if (last_dpad_key & KEY_UP)
				vel_y = -dodge_speed;
			else if (last_dpad_key & KEY_DOWN)
				vel_y = dodge_speed;
		}
		break;
	}

	// secondary status logic.
	switch (secondary_status)
	{
	// JUMPING //
	case S_JUMP:
		Jump();
		break;
	}

	// calculates the distance moved since last idle.

	// resets after some time idle.
	if (frames_idle >= 25)
	{
		distance_ran_x = 0;
		distance_walked_x = 0;
		distance_moved_y = 0;
	}
	else
	{
		distance_ran_x = map_pos_x - start_run_pos_x;
		distance_moved_y = map_pos_y - start_walk_pos_y;
	}

	// key inputs. //

	// left.
	if (key_held & KEY_LEFT && !(key_held & KEY_RIGHT))
	{
		sprite.flipped = true;
		effects[EFF_RUN_EFFECT].flipped = true;
	}
	// right.
	if (key_held & KEY_RIGHT && !(key_held & KEY_LEFT))
	{
		sprite.flipped = false;
		effects[EFF_RUN_EFFECT].flipped = false;
	}

	if (!dodging)
	{
		// down.
		if (key_held & KEY_DOWN && !(key_held & KEY_UP))
		{
			if (!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
				primary_status = P_WALK;
		}
		// up.
		if (key_held & KEY_UP && !(key_held & KEY_DOWN))
		{
			if (!(key_held & KEY_LEFT || key_held & KEY_RIGHT))
				primary_status = P_WALK;
		}
	}

	// idle.
	if (((key_current == 0 || (key_held & KEY_LEFT && key_held & KEY_RIGHT) || (key_held & KEY_UP && key_held & KEY_DOWN))) && !jumping)
	{
		primary_status = P_IDLE;
		can_move = false;
	}

	// jump.
	if (key_pressed & KEY_B || jumping)
		secondary_status = S_JUMP;
	else
		secondary_status = S_NONE;

	// slowdown when character stops moving.

	// horizontal.
	if (((!(key_held & (KEY_LEFT | KEY_RIGHT))) || (key_held & KEY_LEFT && key_held & KEY_RIGHT)) && !jumping)
	{

		slowing_down = true;

		if (vel_x > 0)
			acc_x = -0.2;
		else if (vel_x < 0)
			acc_x = 0.2;
		else
			acc_x = 0;
	}

	if (std::abs(vel_x) <= 0.5f)
	{
		slowing_down = false;
		vel_x = 0;
		acc_x = 0;
	}

	// horizontal limits.
	if (map_pos_x <= -20)
		map_pos_x = -20;
	else if (map_pos_x >= 7690)
		map_pos_x = 7690;

	// vertical limits.
	if (map_pos_y <= 60)
		map_pos_y = 60;
	else if (map_pos_y >= 188)
		map_pos_y = 188;

	// character movement.
	map_pos_x += vel_x;
	map_pos_y += vel_y;
	vel_x += acc_x;
	vel_y += acc_y;

	// adjustment, so the character doesn't stay in the middle of two chunks.
	if (bg_scroll_x % 256 == 0 && bg_scroll_x > 0)
		map_pos_x++;

	// update screen position.
	mapToScreenPos(bg_scroll_x, bg_scroll_y);

	// shadow movement.

	// scales the shadow every 15 frames depending on the jump height.
	if (frames_jumping % 15 == 0)
	{
		shadow.scale_x = 256 + (2 * jump_height);
		shadow.scale_y = 256 + (2 * jump_height);
	}

	// update shadow position.
	shadow.screen_pos_x = sprite.screen_pos_x;
	shadow.screen_pos_y = sprite.screen_pos_y - jump_height + 5 - ((shadow.scale_y - 256) * 0.07f);

	// effects movement.
	for (int i = 0; i < NUM_EFFECTS; i++)
	{
		switch (i)
		{
		case EFF_RUN_EFFECT:
			effects[i].screen_pos_x = sprite.screen_pos_x - distance_ran_x;
			effects[i].screen_pos_y = sprite.screen_pos_y - distance_moved_y - jump_height + 30;
			break;

		case EFF_LAND_EFFECT:
			effects[i].screen_pos_x = sprite.screen_pos_x;
			effects[i].screen_pos_y = sprite.screen_pos_y - jump_height + 30;
			break;
		}
	}

	// did the states change?
	sprite.anim_status_changed = (sprite.past_anim_status != sprite.anim_status);
	primary_status_changed = (past_primary_status != primary_status);
	secondary_status_changed = (past_secondary_status != secondary_status);

	// character frame count.
	frames++;

	// mechanic to let the character move betweens screens. (shitty, now unused)
	if (second_screen_enabled)
	{
		if (map_pos_y >= CHAR_SCREEN_SIZE || sprite.screen_pos_y <= -30)
			sprite.changeScreen();
	}
}

void Character::updateCharacter(int bg_scroll_x, int bg_scroll_y)
{
	moveCharacter(bg_scroll_x, bg_scroll_y);
	sprite.updateSprite();
	shadow.updateSprite();
	for (int i = 0; i < NUM_EFFECTS; i++)
	{
		effects[i].updateSprite();
	}
}

// todo
void Character::mapCollision(LEVELS current_lvl)
{
	switch (current_lvl)
	{
	case L_FROZEN_SUBURBS:

		break;
	}
}

// le constructor.
Character::Character(CHARACTERS id)
{
	char_id = id;
	setupCharacter();
	sprite.setupSprite();
	setupSecondarySprites();
}