#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <math.h>

int read_rom_from_drive(uint8_t* mem, char* rom_loc);
void poll_keys(uint8_t* keys, SDL_Event* event);

int init_video(SDL_Window** window, SDL_Renderer** renderer);
void draw_screen(uint8_t screen[8][32], SDL_Renderer* renderer);
int close_video(SDL_Window* window, SDL_Renderer* renderer);

void play_beep();
void close_audio();
void init_audio();
void stop_beep();