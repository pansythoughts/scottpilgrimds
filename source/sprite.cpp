#include "sprite.h"
#include "libs.h"

// le methods.

	// assigns directories.
    void Sprite::assignSpriteDirs()
    {
	// directories are assigned depending on the sprite type.
	switch (type)
	{
		case SPR_CHARACTER:

		for(int i = 0; i < num_sprites; i++)
		{
	    sprite_dir[i] = "sprite/" + sprite_name;	
	    palette_dir[i] = "palette/" + sprite_name;
	    switch(i)
		{
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
		
		case SPR_SHADOW:

		sprite_dir[0] = "sprite/shadow";
		palette_dir[0] = "palette/shadow";
		break;

		case SPR_RUN_EFFECT:
		sprite_dir[0] = "sprite/run_effect";
		palette_dir[0] = "palette/run_effect";
		break;

		case SPR_LAND_EFFECT:
		sprite_dir[0] = "sprite/land_effect";
		palette_dir[0] = "palette/land_effect";
		break;
	}
}
    
    // loads all sprites to RAM.
    void Sprite::assignSpritesRAM()
	{
		for(int j = 0; j < num_sprites; j++)
		{
			// searches for a free RAM slot, and loads the sprite data to it.
		    for(int i = 0; i < NF_SLOTS_SPR256GFX; i++)
		        if(NF_SPR256GFX[i].available)
				{
	                NF_LoadSpriteGfx(sprite_dir[j].c_str(), i, size_x, size_y);
					gfx_id[j] = i;
				    break;
				}

		    // searches for a free RAM slot, and loads the palette data to it.
		    for(int i = 0; i < NF_SLOTS_SPR256PAL; i++)
		        if(NF_SPR256PAL[i].available)
				{
	                NF_LoadSpritePal(palette_dir[j].c_str(), i);
					pal_id[j] = i;
				 	break;
				}
		}
	}

	// loads the sprite and palette to VRAM.
    void Sprite::assignSpritesVRAM()
	{
		// looks for a free sprite slot in VRAM.
		for(int i = 0; i < 128; i++)
		if(!NF_SPR256VRAM[screen][i].inuse)
		{
	    NF_VramSpriteGfx(screen, gfx_id[anim_status], i, true);
		gfx_vram_id[anim_status] = i;
		break;
		}    
		// looks for a free palette slot in VRAM.   
		for(int i = 0; i < 16; i++)
		if(!NF_SPRPALSLOT[screen][i].inuse)
		{                                                  
	    NF_VramSpritePal(screen, pal_id[anim_status], i); 
		pal_vram_id[anim_status] = i;
		break;
		}
	}

	// frees memory used by the sprite and palette.
	void Sprite::freeSpritesRAM()
	{
	    // free from RAM.
		for(int i = 0; i < num_sprites; i++)
		{
	    NF_UnloadSpriteGfx(gfx_id[i]);
	    NF_UnloadSpritePal(pal_id[i]);
		}
		
	}

	void Sprite::freeSpritesVRAM()
	{
		//free from VRAM.
	    NF_FreeSpriteGfx(screen, gfx_vram_id[anim_status]);
		NF_SPR256VRAM[screen][gfx_vram_id[anim_status]].inuse = false;
		NF_SPRPALSLOT[screen][pal_vram_id[anim_status]].inuse = false;
	}

	// changes sprite from a screen to another, mantaining its relative position. (shitty mechanic.)
	void Sprite::changeScreen()
	{
	    if (screen_pos_y >= CHAR_SCREEN_SIZE)
			screen_pos_y = CHAR_SCREEN_SIZE - screen_pos_y;
		else if (screen_pos_y <= -30)
		    screen_pos_y = CHAR_SCREEN_SIZE + screen_pos_y;
			
			NF_DeleteSprite(screen, slot_id);
		    freeSpritesVRAM();
		    if (screen == 0) screen = 1;
		    else screen = 0;
		    assignSpritesVRAM();
		    createSprite();
	}

    // creates/shows a SINGLE sprite in screen.
    void Sprite::createSprite()
	{
		// searches for a free OAM slot, and load the sprite to it.
		for(int i = 0; i < 128; i++)
		{
			if(!NF_SPRITEOAM[screen][i].created)
			{
				NF_CreateSprite(screen, i, gfx_vram_id[anim_status], pal_vram_id[anim_status], screen_pos_x, screen_pos_y);
				if(rotscale)
				NF_EnableSpriteRotScale(screen, i, i, false);
				sprite_id[anim_status] = i;
				break;
			}

		}
		created = true;
	}

	void Sprite::deleteSprite()
	{
		// deletes le sprite.
		NF_DeleteSprite(screen, sprite_id[anim_status]);
		created = false;
	}

	// creates a mirrored sprite. (like the landing effect, for example).
	void Sprite::createMirroredSprite()
	{
		if(mirrored)
		{
			// creates first half.
		    createSprite();
		    flipped = true;
			
			// creates the other half in another sprite slot (probably the next but not guaranteed lol)
		    for(int i = 0; i < 128; i++)
		    {
			    if(!NF_SPRITEOAM[screen][i].created)
			    {
				NF_CreateSprite(screen, i, gfx_vram_id[anim_status], pal_vram_id[anim_status], screen_pos_x + 32, screen_pos_y);
				NF_HflipSprite(screen, i, false);
				break;
			    }

		    }
	    }

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
			if(anim_frame > n_frames[anim_status])
			{
				anim_frame = 0;
				if(!looped)
				{
				    deleteSprite();
				    if(mirrored)
				    NF_DeleteSprite(screen, sprite_id[anim_status] + 1); // risky, but maybe always works?
				}
			}
			NF_SpriteFrame(screen, sprite_id[anim_status], anim_frame); //screen, id, frame.
		}
	}
}
    //updates the sprite and palette to the one according to the current animation state.
    void Sprite::updateSpriteAndPalette()
	{
		// lol.
	    deleteSprite();
	    createSprite();
    }

    // updates the whole sprite (position, animation, sprite, palette, etc.)
    void Sprite::updateSprite()
	{
		if(created)
		{

	    // updates the sprite and palette when needed.
		if(anim_status_changed)
		{
		freeSpritesVRAM();
		assignSpritesVRAM();
		updateSpriteAndPalette();
		}

		// moves and rotates the sprite.
		if(can_move)
		NF_MoveSprite(screen, sprite_id[anim_status], screen_pos_x, screen_pos_y);

		if(rotscale)
		NF_SpriteRotScale(screen, sprite_id[anim_status], rot, scale_x, scale_y);

        // sprite flip.
		NF_HflipSprite(screen, sprite_id[anim_status], flipped);


		animateSprite();
        }
    }

	void Sprite::freePalVRAM()//SHHHITTYYY DONT USE
	{
		u32 address = 0x06890000 + (pal_vram_id[anim_status] * 256 * 2);
        memset((void *)address, 0, NF_SPR256PAL[pal_id[anim_status]].size);
		NF_SPRPALSLOT[screen][pal_vram_id[anim_status]].inuse = false;

		NF_SPRVRAM[screen].free += NF_SPR256PAL[pal_id[anim_status]].size;
		NF_SPRVRAM[screen].pos[NF_SPRVRAM[screen].deleted] = address;
		NF_SPRVRAM[screen].size[NF_SPRVRAM[screen].deleted] = NF_SPR256PAL[pal_id[anim_status]].size;
		NF_SPRVRAM[screen].deleted++;
		NF_SPRVRAM[screen].fragmented += NF_SPR256PAL[pal_id[anim_status]].size;
	}

    // sets up sprite.
    void Sprite::setupSprite()
    {
		switch(type)
		{
			case SPR_RUN_EFFECT:
			size_x = 32;
			size_y = 32;
			animated = true;
			num_sprites = 1;
			looped = false;
			n_frames[0] = 4;
			anim_frame_period[0] = 4;
			can_move = true;
			rotscale = false;
			break;

			case SPR_LAND_EFFECT:
			size_x = 32;
			size_y = 32;
			animated = true;
			num_sprites = 1;
			looped = false;
			n_frames[0] = 4;
			anim_frame_period[0] = 4;
			can_move = false;
			mirrored = true;
			rotscale = false;
			break; 

			case SPR_SHADOW:
			size_x = 64;
			size_y = 64;
			animated = false;
			num_sprites = 1;
			looped = false;
			n_frames[0] = 0;
			anim_frame_period[0] = 0;
			can_move = true;
			rotscale = true;
			break;
		}
	    assignSpriteDirs();
	    assignSpritesRAM();
		assignSpritesVRAM();
    }