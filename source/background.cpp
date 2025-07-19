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
        layers[LEFT] = 0;
        layers[RIGHT] = 1;
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

    // unload them from RAM.
    NF_UnloadTiledBg(bg_dirs[chunk].c_str());
    NF_UnloadTiledBg(bg_dirs[chunk + 1].c_str());

    // wipes the right half of both backgrounds. avoids glitchy graphics on the empty half.
    deleteBGRightHalf(layers[0]);
    deleteBGRightHalf(layers[1]);

}

void Background::deleteBackground()
{
    // deletes both layers of background.
    NF_DeleteTiledBg(screen, layers[0]);
    NF_DeleteTiledBg(screen, layers[1]);
}

// scroll system.
void Background::updateScroll(int char_map_pos_x, int char_map_pos_y)
{
    if(scrolled)
    {
        int left_layer, right_layer;

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
            chunk_scroll_x = map_scroll_x - (chunk * SCREEN_WIDTH);
        }


        // scroll the background.
        if(!layers_swapped)
        {
            left_layer = layers[0];
            right_layer = layers[1];
        }
        else
        {
            left_layer = layers[1];
            right_layer = layers[0];
        }
        scrollNoLoop(screen, left_layer, chunk_scroll_x, chunk_scroll_y);
        scrollNoLoop(screen, right_layer, -SCREEN_WIDTH + chunk_scroll_x, chunk_scroll_y);

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
    


    // helper variables.
    bool load_right_chunk = prev_scroll_x < SCREEN_WIDTH && chunk_scroll_x >= SCREEN_WIDTH;
    bool load_left_chunk = prev_scroll_x > 0 && chunk_scroll_x <= 0;


    // if a new chunk must be loaded...
    if(((load_right_chunk) || (load_left_chunk)) && map_scroll_x > 0)
    {
        // helper variable.
        int layer_to_delete;
  
        // next chunk.
        if(load_right_chunk)
        {
        chunk++;
        new_right_chunk = true;
        }
        else
        {
        chunk--;
        new_left_chunk = true;
        }

        new_chunk = true;

        // if layer 0 = left, and layer 1 = right...
        if(!layers_swapped)
        {
            layer_to_delete = (load_right_chunk)? layers[0] : layers[1];
        }
        else
        {
            layer_to_delete = (load_left_chunk)? layers[0] : layers[1];
        }

        // load previous, current and next chunks to RAM.
        if(chunk > 0)
        loadChunk(chunk - 1);

        loadChunk(chunk);
        loadChunk(chunk + 1);

        // delete and create the needed chunk in the correspondant layer.
        NF_DeleteTiledBg(screen, layer_to_delete);
        NF_CreateTiledBg(screen, layer_to_delete,
                        (load_right_chunk)? 
                        bg_dirs[chunk + 1].c_str() :  bg_dirs[chunk].c_str());

        // swap the layers order after each chunk is loaded.
        layers_swapped = !layers_swapped;
        
        // clean the right half of each chunk.
        deleteBGRightHalf(layers[0]);
        deleteBGRightHalf(layers[1]);
    }
    else
    {
        // no new chunk :c
        new_left_chunk = false;
        new_right_chunk = false;
        new_chunk = false;
    }

    // updates le scroll.
    updateScroll(char_map_pos_x, char_map_pos_y);
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

// loads a chunk of background to RAM.
void Background::loadChunk(int chunk_id)
{
    // if it's already loaded...
    if(loaded_chunks.count(chunk_id))
    {
        chunk_order.remove(chunk_id);
        chunk_order.push_front(chunk_id);
    }
    else
    {
        // load it to RAM and add it to the loaded chunks map.
        NF_LoadTiledBg(bg_dirs[chunk_id].c_str(), bg_dirs[chunk_id].c_str(), size_x, size_y);
        loaded_chunks[chunk_id] = bg_dirs[chunk_id];
        chunk_order.push_front(chunk_id);
    }

    if(loaded_chunks.size() > 3) // <- max amount of chunks in RAM.
    {
        // unload the least used one.
        chunk_order.pop_back();
        NF_UnloadTiledBg(loaded_chunks[chunk_order.back()].c_str());
        loaded_chunks.erase(chunk_order.back());
    }
}


Background::Background(int bg_id)
{
    bg = bg_id;
    setupBackground();
}