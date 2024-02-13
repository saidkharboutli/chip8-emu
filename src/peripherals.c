#include "peripherals.h"

#define SCALE 16

/**
 * @file peripherals.c
 * @author Said Kharboutli (said.kharboutli5@gmail.com)
 * @brief Contains code for the chip8 CPU peripherals.
 * @date 2024-01-05
 * 
 * @cite https://glusoft.com/sdl2-tutorials/install-sdl-linux/
 * @cite https://stackoverflow.com/a/45002609 (for audio)
 */

void PrintKeyInfo( SDL_KeyboardEvent *key ){
        /* Is it a release or a press? */
        if( key->type == SDL_KEYUP )
            printf( "Release:- " );
        else
            printf( "Press:- " );

        /* Print the hardware scancode first */
        printf( "Scancode: 0x%02X", key->keysym.scancode );
        /* Print the name of the key */
        printf( ", Name: %s\n", SDL_GetKeyName( key->keysym.sym ) );
    }

/* DATA SPACE */
/* * * * * * * * * * * * * * */

// Key mappings for SDL
SDL_Keycode key_map[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, // 0 1 2 3
    SDLK_q, SDLK_w, SDLK_e, SDLK_a, // 4 5 6 7
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, // 8 9 A B
    SDLK_4, SDLK_r, SDLK_f, SDLK_v, // C D E F
};

/* * * * * * * * * * * * * * */


/* ROM DRIVE */
/* * * * * * * * * * * * * * */

int read_rom_from_drive(uint8_t* mem, char* rom_loc) {
    FILE *fp;
    int i = 0x200;
    int lo, hi;

    if(!(fp = fopen(rom_loc, "rb"))) return -1;
    while((lo=fgetc(fp)) != EOF && i < 4096) {
        hi = fgetc(fp);
        mem[i++] = lo;
        mem[i++] = hi;
    }

    fclose(fp);
    return i;
}

/* * * * * * * * * * * * * * */


/* AUDIO */
/* * * * * * * * * * * * * * */

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;

void init_audio() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
}

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int* sample_nr = (int*)user_data;

    for(int i = 0; i < length; i++) {
        double time = (double)sample_nr[i] / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * 3.1415f * 441.0f * time)); // render 441 HZ sine wave
    }
}

void play_beep() {
    int sample_nr = 0;

    SDL_AudioSpec want;
    want.freq = SAMPLE_RATE; // number of samples per second
    want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
    want.channels = 1; // only one channel
    want.samples = 512; // buffer-size
    want.callback = audio_callback; // function SDL calls periodically to refill the buffer
    want.userdata = &sample_nr; // counter, keeping track of current sample number

    SDL_AudioSpec have;
    if(SDL_OpenAudio(&want, &have) != 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
    if(want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");

    SDL_PauseAudio(0); // start playing sound
}

void stop_beep() {
    SDL_PauseAudio(1);
}

void close_audio() {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

/* * * * * * * * * * * * * * */

/* KEYBOARD */
/* * * * * * * * * * * * * * */

void poll_keys(uint8_t* keys, SDL_Event* event) {
    for(int i = 0; i < 16; i++) {
        if(event->key.keysym.sym == key_map[i]) {
            PrintKeyInfo(&event->key);
            if(event->type == SDL_KEYDOWN) {
                keys[i] = 1;
                printf("%02x\n", i);
            } else if(event->type == SDL_KEYUP) {
                keys[i] = 0;
            }
        }
    }
}

/* * * * * * * * * * * * * * */


/* VIDEO DRIVER */
/* * * * * * * * * * * * * * */

int init_video(SDL_Window** window, SDL_Renderer** renderer) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("CHIP8 EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * SCALE, 32 * SCALE, 0);
    if(!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

void draw_screen(uint8_t screen[8][32], SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for(int i = 0; i < 32; i++) {
        for(int j = 0; j < 8; j++) {
            for(int k = 0; k < 8; k++) {
                if(screen[j][i] & (0b10000000 >> k)) {
                    SDL_Rect rect;
                    rect.x = ((j * 8) + k) * SCALE;
                    rect.y = i * SCALE;
                    rect.w = SCALE;
                    rect.h = SCALE;
                    SDL_RenderFillRect(renderer, &rect);
                    // O avg = 32 * 64, no larger than any other implementation.
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int close_video(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/* * * * * * * * * * * * * * */