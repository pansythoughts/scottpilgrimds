#include "libs.h"
#include "enums.h"
#include "sound.h"


void Song::setupSong()
{
    switch(song_id)
    {
        case ANOTHER_WINTER:

        n_of_files = 4;
        last_chunk_duration = 17;
        break;
    }
}

// assigns le directories.
void Song::assignSongDirs()
{
    for(int i = 0; i < n_of_files; i++)
    {
        switch(song_id)
        {
            case ANOTHER_WINTER:
            song_dirs[i] = "songs/another_winter_" + std::to_string(i);
            break;
        }
    }
}

void Song::loadChunkRAM()
{
    // looks for an available slot for the chunk.
    for(int i = 0; i < NF_SLOTS_RAWSOUND; i++)
    {
        if(NF_RAWSOUND[i].available)
        {
            NF_LoadRawSound(song_dirs[chunk].c_str(), i, 11025, 0);
            slot_id = i;
            break;
        }
    }

}

void Song::unloadChunkRAM()
{
    // self explanatory.
    NF_UnloadRawSound(slot_id);
}

void Song::playSong()
{
    int current_chunk_duration = (chunk < n_of_files - 1)? chunk_duration : last_chunk_duration;

    if(!playing || chunk_time_passed >= current_chunk_duration)
    {
       
        if(chunk_time_passed >= current_chunk_duration)
        {
            // new chunk of song.
            chunk++;
            // if on last chunk, loop.
            if(current_chunk_duration == last_chunk_duration)
            chunk = 0;

            chunk_time_passed = 0;

            unloadChunkRAM();
            loadChunkRAM();
        }
        
        playChunk();
        playing = true;
    }

    // kinda frame counter.
    timer++;

    // seconds the chunk has played. (assumes 60 fps).
    if(timer % 60 == 0)
    chunk_time_passed++;

}

void Song::stopSong()
{
    if(playing)
    {
        unloadChunkRAM();
        NF_ResetRawSoundBuffers();
        playing = false;
    }
}

void Song::playChunk()
{
    NF_PlayRawSound(slot_id, 127, 64, false, 0);
}

Song::Song(SONGS id)
{
    song_id = id;
    setupSong();
    assignSongDirs();
}
