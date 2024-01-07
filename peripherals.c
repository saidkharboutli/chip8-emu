#include "peripherals.h"

/**
 * @file peripherals.c
 * @author Said Kharboutli (said.kharboutli5@gmail.com)
 * @brief Contains code for the chip8 CPU peripherals.
 * @date 2024-01-05
 */

/* ROM DRIVE */
/* * * * * * * * * * * * * * */


// hello
int read_rom_from_drive(uint8_t* mem, char* rom_loc) {
    FILE *fp;
    int i = 0x200;
    int b;

    if(!(fp = fopen(rom_loc, "rb"))) return -1;
    while((b=fgetc(fp)) != EOF && i < 4096) {
        mem[i++] = b;
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


/* VIDEO DRIVER V1 */
/* * * * * * * * * * * * * * */

void draw_screen(uint8_t screen[64][32]) {
    system("clear");
    for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 32; j++) {
            if(screen[i][j]) printf("#");
            else printf(".");
        }
        printf("\n");
    }
}

/* * * * * * * * * * * * * * */