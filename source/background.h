#pragma once

#include "libs.h"
#include "enums.h"


class Background
{
    public:
    int bg = 0;
    std::string bg_chunks[2];
    std::string bg_name = "";
    std::string bg_dirs[32];
    int n_of_files = 0;
    bool scrolled = false;
    int size_x = 0;
    int size_y = 0;
    int layers[2];
    int screen = 0;
    int map_scroll_x = 0;
    int prev_scroll_x = 0;
    int map_scroll_y = 0;
    int chunk_scroll_x = 0;
    int chunk_scroll_y = 0;
    int min_scroll_value = 0;
    int max_scroll_value = 0;
    int scale_x = 0;
    int scale_y = 0;
    int chunk = 0;
    bool layers_swapped = false;
    bool new_chunk = false;
    bool new_left_chunk = false;
    bool new_right_chunk = false;
    bool new_left_chunk_loaded = false;
    bool new_right_chunk_loaded = false;

    std::map<int, std::string> loaded_chunks;
    std::list<int> chunk_order;

    void setupBackground();
    void createBackground();
    void deleteBackground();
    void scrollNoLoop(int screen, int layer, int scroll_x, int scroll_y);
    void updateScroll(int char_map_pos_x, int char_map_pos_y);
    void updateBackground(int char_map_pos_x, int char_map_pos_y);
    void deleteBGRightHalf(int layer);
    void loadChunk(int chunk_id);

    Background(int bg_id);
};