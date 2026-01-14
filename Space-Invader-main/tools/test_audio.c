#define MINIAUDIO_IMPLEMENTATION
#include "../src/utils/miniaudio.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    ma_engine engine;
    ma_result result;
    
    printf("Initializing Miniaudio Standalone...\n");
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to init engine: %d\n", result);
        return -1;
    }
    // printf("Engine initialized. Backend: %d\n", ma_engine_get_device(&engine)->backendType);
    printf("Engine initialized.\n");
    
    // Check master volume
    printf("Master Volume: %.2f\n", ma_engine_get_volume(&engine));
    
    printf("Playing bin/assets/music_game.wav...\n");
    result = ma_engine_play_sound(&engine, "bin/assets/music_game.wav", NULL);
    if (result != MA_SUCCESS) {
        printf("Failed to play sound: %d\n", result);
    } else {
        printf("Sound started. Waiting 5 seconds...\n");
        sleep(5);
    }
    
    ma_engine_uninit(&engine);
    printf("Done.\n");
    return 0;
}
