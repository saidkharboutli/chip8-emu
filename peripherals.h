#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int read_rom_from_drive(uint8_t* mem, char* rom_loc);
uint8_t poll_keys(uint8_t* keys);
void draw_screen(uint8_t** screen);