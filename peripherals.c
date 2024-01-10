#include "peripherals.h"

/**
 * @file peripherals.c
 * @author Said Kharboutli (said.kharboutli5@gmail.com)
 * @brief Contains code for the chip8 CPU peripherals.
 * @date 2024-01-05
 * 
 * @cite https://glusoft.com/sdl2-tutorials/install-sdl-linux/
 */

/* ROM DRIVE */
/* * * * * * * * * * * * * * */

int read_rom_from_drive(uint8_t* mem, char* rom_loc) {
    FILE *fp;
    int i = 0x200;
    int lo, hi;

    if(!(fp = fopen(rom_loc, "rb"))) return -1;
    while((lo=fgetc(fp)) != EOF && i < 4096) {
        hi = fgetc(fp);
        mem[i++] = hi;
        mem[i++] = lo;
    }

    fclose(fp);
    return i;
}

/* * * * * * * * * * * * * * */


/* KEYBOARD V1 */
/* * * * * * * * * * * * * * */

uint8_t poll_keys(uint8_t* keys) {
    uint8_t pressed = 16;
    char key = getchar();
    char* mapping = "1234qwerasdfzxcv";
    for(int i = 0; mapping[i] != '\0'; i++) {
        if(key == mapping[i]) keys[pressed=i] = 1;
        else keys[i] = 0;
    }

    return pressed;
}

/* * * * * * * * * * * * * * */


/* VIDEO DRIVER */
/* * * * * * * * * * * * * * */

int init_video(SDL_Window** window, SDL_Renderer** renderer) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("CHIP8 EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
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
                    rect.x = ((j * 8) + k) * 10;
                    rect.y = i * 10;
                    rect.w = 10;
                    rect.h = 10;
                    SDL_RenderFillRect(renderer, &rect);
                    // printf("%d", (screen[j][i] & (0b10000000 >> k)) >> (7 - k));
                }
                // else printf(" ");
                // printf(" ");
            }
        }
        // printf("\n");
    }
    // printf("---");
    SDL_RenderPresent(renderer);
}

int close_video(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/* * * * * * * * * * * * * * */