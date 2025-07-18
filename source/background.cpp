#include "libs.h"
#include "background.h"
#include "input.h"

void Background::setupBackground()
{
    switch(bg)
    {
        case FROZEN_SUBURBS:

        bg_name = "frozen_suburbs";
        bg_chunks[LEFT] = "frozen_suburbs_left";
        bg_chunks[RIGHT] = "frozen_suburbs_right";
        n_of_files = 32;
        size_x = 512;
        size_y = 256;
        scrolled = true;
        layers[LEFT] = 1;
        layers[RIGHT] = 0;
        screen = 0;
        for(int i = 0; i < n_of_files; i++)
        {
            bg_dirs[i] = "bg/frozen_suburbs_" + std::to_string(i);
        }
        break;
    }

}

void Background::createBackground()
{
    // reserves memory for second (right) chunk of loaded background.
    // IMPORTANT: this WILL TAKE the memory that was originally destinated to
    // the sub screen main BG, so the second screen WILL NOT be able to use
    // backgrounds nor show text; only sprites will be available.
    // DELETE IF CONSOLE DEBUG TEXT WANTED IN SUB SCREEN. (the right BG chunk will half-load as a result.)
    vramSetBankC(VRAM_C_MAIN_BG_0x06020000);
    

    // loads and creates left chunk.
    NF_LoadTiledBg(bg_dirs[chunk].c_str(), bg_dirs[chunk].c_str(), size_x, size_y);
	NF_CreateTiledBg(screen, layers[LEFT], bg_dirs[chunk].c_str());

    // loads and creates right chunk.
    NF_LoadTiledBg(bg_dirs[chunk + 1].c_str(), bg_dirs[chunk + 1].c_str(), size_x, size_y);
    NF_CreateTiledBg(screen, layers[RIGHT], bg_dirs[chunk + 1].c_str());

    // wipes the right half of both backgrounds. avoids glitchy graphics on the empty half.
    deleteBGRightHalf(layers[LEFT]);
    deleteBGRightHalf(layers[RIGHT]);

}

void Background::deleteBackground()
{

    // deletes both layers of background.
    NF_DeleteTiledBg(screen, layers[LEFT]);
    NF_UnloadTiledBg(bg_dirs[chunk].c_str());

    NF_DeleteTiledBg(screen, layers[RIGHT]);
    NF_UnloadTiledBg(bg_dirs[chunk + 1].c_str());

}

// scroll system.
void Background::updateScroll(int char_map_pos_x, int char_map_pos_y)
{
    if(scrolled)
    {
        // caches scroll value in x to compare in future frames.
        prev_scroll_x = chunk_scroll_x;


        // determine scroll value for x and y.
        if(char_map_pos_y >= 70 && char_map_pos_y <= 130)
        {
            map_scroll_y = char_map_pos_y - 70;
            chunk_scroll_y = map_scroll_y;
        }

        if(char_map_pos_x >= 100)
        {
            map_scroll_x = char_map_pos_x - 100;

            chunk_scroll_x = map_scroll_x - ((chunk) * SCREEN_WIDTH);
        }


        // scroll the background.
        scrollNoLoop(screen, layers[LEFT], chunk_scroll_x, chunk_scroll_y);
        scrollNoLoop(screen, layers[RIGHT], -SCREEN_WIDTH + chunk_scroll_x, chunk_scroll_y);

    }
}

void Background::scrollNoLoop(int screen, int layer, int scroll_x, int scroll_y)
{
    
    if(screen == 0)
    {
        // main screen.
        switch(layer)
        {
            // update scroll registers of the used layer.
            case 0:
                REG_BG0HOFS = scroll_x;
                REG_BG0VOFS = scroll_y;
                break;
            case 1:
                REG_BG1HOFS = scroll_x;
                REG_BG1VOFS = scroll_y;
                break;
            case 2:
                REG_BG2HOFS = scroll_x;
                REG_BG2VOFS = scroll_y;
                break;
            case 3:
                REG_BG3HOFS = scroll_x;
                REG_BG3VOFS = scroll_y;
                break;
        }
    }
    else
    {
        // sub screen.
        switch (layer)
        {
            // same.
            case 0:
                REG_BG0HOFS_SUB = scroll_x;
                REG_BG0VOFS_SUB = scroll_y;
                break;
            case 1:
                REG_BG1HOFS_SUB = scroll_x;
                REG_BG1VOFS_SUB = scroll_y;
                break;
            case 2:
                REG_BG2HOFS_SUB = scroll_x;
                REG_BG2VOFS_SUB = scroll_y;
                break;
            case 3:
                REG_BG3HOFS_SUB = scroll_x;
                REG_BG3VOFS_SUB = scroll_y;
                break;
        }
    }

}

void Background::updateBackground(int char_map_pos_x, int char_map_pos_y)
{
    
    // updates le scroll.
    updateScroll(char_map_pos_x, char_map_pos_y);

    // if a new chunk must be loaded...
    if(((prev_scroll_x < SCREEN_WIDTH && chunk_scroll_x >= SCREEN_WIDTH) || (prev_scroll_x > 0 && chunk_scroll_x <= 0)) && map_scroll_x > 0)
    {

        // deletes all bgs.
        deleteBackground();
      
        // next chunk.
        if(prev_scroll_x < SCREEN_WIDTH && chunk_scroll_x >= SCREEN_WIDTH)
        chunk++;
        else
        chunk--;

        new_chunk = true;
        
        // load the former right chunk to the left layer, as the new left chunk.
        createBackground();
    }
    else
        // no new chunk :c
        new_chunk = false;

}

// deletes the map corresponding to the right half of a 512 x 256 background.
// REALLY USEFUL for my BG scroll system, since the empty half tends to appear glitchy.
void Background::deleteBGRightHalf(int layer)
{
    // creates pointer to the position in memory where the maps begin to be stored.
    u16* map = (u16*)(0x6000000 + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11));

    // sets to 0 the positions corresponding to the right half.
    for(int i = 1025; i < 2048; i++)
        *(map + i) = 0;
}


Background::Background(int bg_id)
{
    bg = bg_id;
    setupBackground();
}