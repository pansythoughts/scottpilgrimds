#include "sprite.h"
#include "libs.h"

// le methods.

	// assigns directories.
    void Sprite::assignSpriteDirs()
    {

	// directories are assigned depending on the sprite type.
	switch (type)
	{
		case CHARACTER:
		for(int i = 0; i < CHAR_NUM_SPRITES; i++)
		{
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
		break;
		
		case SHADOW:
		sprite_dir[0] = "sprite/shadow";
		palette_dir[0] = "palette/shadow";
		break;
	}
}
    
    // loads sprites to RAM, then VRAM. 
	//OJO: posible causante del error 'Data abort.' cuando se ejecuta en hardware real.
	//(puede que haya overflow de memoria(?))
	//^
	//|
	//MENTIROTA ALABERGA. SE CAUSABA POR VARIABLES NO INICIALIZADAS.
    void Sprite::assignSpritesMemory()
	{
	// loads a sprite and its palette to RAM (id = ram_slot).
	NF_LoadSpriteGfx(sprite_dir[anim_status].c_str(), id, size_x, size_y);//(file, RAM slot, size x, size y) 
	NF_LoadSpritePal(palette_dir[anim_status].c_str(), id);//(file, RAM slot)(c_str() para convertir de std::string a char[])

    // loads the sprite and palette to VRAM.
	NF_VramSpriteGfx(screen, id, id, true);//(screen, RAM slot, VRAM slot, copy all frames to VRAM)                                                          
	NF_VramSpritePal(screen, id, id);      //(screen, RAM slot, VRAM slot)
	}

	// frees memory used by the sprite and palette.
	void Sprite::freeSpritesMemory()
	{
	NF_UnloadSpriteGfx(id);
	NF_UnloadSpritePal(id);
	NF_FreeSpriteGfx(screen, id);
	}

	// changes sprite from a screen to another, mantaining its relative position.
	void Sprite::changeScreen()
	{
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

    // creates/shows a sprite in screen.
    void Sprite::createSprite()
	{
	NF_CreateSprite(screen, id, id, id, screen_pos_x, screen_pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
	NF_EnableSpriteRotScale(screen, id, id, false);//(screen, sprite ID, rot ID, 'doublesize') <--- por algun motivo desplaza el sprite un poco abajo a la derecha.
	}

	// animates a sprite (if it's animated).
    void Sprite::animateSprite()
	{
		if (animated){
		// animation logic.
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
    //updates the sprite and palette to the one according to the current animation state.
    void Sprite::updateSpriteAndPalette()
	{
	NF_DeleteSprite(screen, id);
	NF_CreateSprite(screen, id, id, id, screen_pos_x, screen_pos_y);//(screen, RAM Slot, VRAM slot SPRITE, VRAM slot PALETTE, pos x, pos y)
    }

    // updates the whole sprite (position, animation, sprite, palette, etc.)
    void Sprite::updateSprite()
	{
		if(anim_status >= 0 && anim_status <= CHAR_NUM_SPRITES){

	    // updates the sprite and palette when needed.
		if(anim_status_changed)
		{
		freeSpritesMemory();
		assignSpritesMemory();
		updateSpriteAndPalette();
		}

		// moves and rotates the sprite.
		NF_MoveSprite(screen, id, screen_pos_x, screen_pos_y);
		NF_SpriteRotScale(screen, id, rot, scale_x, scale_y);

		// sprite flip.
		if(flipped)
		NF_HflipSprite(screen, id, true);
		else
		NF_HflipSprite(screen, id, false);

		animateSprite();
        }
    }
    // sets up sprite.
    void Sprite::setupSprite()
    {
	    assignSpriteDirs();
	    assignSpritesMemory();
	    createSprite();
    }