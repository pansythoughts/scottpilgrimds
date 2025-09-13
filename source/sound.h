#pragma once

#include "libs.h"
#include "enums.h"

class Song
{
    public:

    int chunk_duration = 20; // seconds;
    int last_chunk_duration = 0;

    int n_of_files = 0;
    std::string song_dirs[SONG_MAX_FILES];
    SONGS song_id;
    int slot_id = 0;
    int chunk = 0;
    volatile int timer = 0;
    int chunk_time_passed = 0;
    bool playing = false;


    void setupSong();
    void loadChunkRAM();
    void unloadChunkRAM();
    void assignSongDirs();
    void playSong();
    void stopSong();
    void playChunk();
    void stopChunk();
    Song(SONGS);
};