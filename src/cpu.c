/**
 * @file cpu.c
 * @author Said Kharboutli (said.kharboutli5@gmail.com)
 * @brief Defines the main logic that makes up the chip8 CPU.
 * @date 2024-01-04
 * 
 * @cite http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.1
 * @cite https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
 * @cite http://johnearnest.github.io/Octo/docs/chip8ref.pdf
 */

/*
    TODO:
    2. outputting graphics
    3. testing
*/

#include "peripherals.h"
#include <time.h>

/* DATA SPACE */
/* * * * * * * * * * * * * * */

/* Sprite Data */
uint8_t sprite[16][5] = {
    { 0xF0, 0x90, 0x90, 0x90, 0xF0 },
    { 0x20, 0x60, 0x20, 0x20, 0x70 },
    { 0xF0, 0x10, 0xF0, 0x80, 0xF0 },
    { 0xF0, 0x10, 0xF0, 0x10, 0xF0 },
    { 0x90, 0x90, 0xF0, 0x10, 0x10 },
    { 0xF0, 0x80, 0xF0, 0x10, 0xF0 },
    { 0xF0, 0x80, 0xF0, 0x90, 0xF0 },
    { 0xF0, 0x10, 0x20, 0x40, 0x40 },
    { 0xF0, 0x90, 0xF0, 0x90, 0xF0 },
    { 0xF0, 0x90, 0xF0, 0x10, 0xF0 },
    { 0xF0, 0x90, 0xF0, 0x90, 0x90 },
    { 0xE0, 0x90, 0xE0, 0x90, 0xE0 },
    { 0xF0, 0x80, 0x80, 0x80, 0xF0 },
    { 0xE0, 0x90, 0x90, 0x90, 0xE0 },
    { 0xF0, 0x80, 0xF0, 0x80, 0xF0 },
    { 0xF0, 0x80, 0xF0, 0x80, 0x80 }
};

/* * * * * * * * * * * * * * */


/* MEMORY SPACE */
/* * * * * * * * * * * * * * */

uint8_t mem[4096];      /* Memory Block */

uint16_t stack[16];     /* Stack */

/* Pseudo-Registers */
uint16_t PC = 0x200;    /* Program Counter */
uint8_t SP = 0x0;         /* Stack Pointer */

/* Special Purpose Registers */
uint8_t DT, ST;         /* Delay Timer, Sound Timer */

/* GP Registers */
uint8_t V[16];
uint16_t I;             /* Memory Register */

/* Video Buffer */
uint8_t screen[8][32];

/* Input Buffer */
uint8_t key[16];

/* Instruction Bus */
uint16_t instr_bus;

/* * * * * * * * * * * * * * */


/* EXECUTION SPACE */
/* * * * * * * * * * * * * * */

uint16_t fetch() {
    instr_bus = (mem[PC]<<8) | mem[PC+1];
    PC += 2;
    return instr_bus;
}

uint16_t decode(uint16_t instr) {
    if(DT > 0) DT--;
    if(ST > 0) {
        ST--;
    }

    return (instr&0xf000) >> 12;
}

void execute(uint8_t instr_t) {
    uint8_t x = (instr_bus&0x0F00)>>8, y = (instr_bus&0x00F0)>>4, imm = (instr_bus&0x00FF);
    uint8_t prev = V[x];
    uint16_t addr = (instr_bus&0x0FFF);

    switch(instr_t){
        case 0x0:
            if(instr_bus == 0x00EE) {
                /* ret / exit subroutine */
                PC = stack[--SP];
            } else if(instr_bus == 0x00E0) {
                /* cls / clear display */
                for (uint8_t i = 0; i < 8; i++) for(int j = 0; j < 32; j++) screen[i][j] = 0;
            }
            break;
        case 0x1:
            /* jmp / jump to NNN */
            PC = addr;
            break;
        case 0x2:
            /* jal / call NNN */
            stack[SP++] = PC;
            PC = addr;
            break;
        case 0x3:
            /* snei / if Vx != kk then skip next instr */
            if(V[x] == imm) PC += 2;
            break;
        case 0x4:
            /* seqi / if Vx == kk then skip next instr */
            if(V[x] != imm) PC += 2;
            break;
        case 0x5:
            /* sne / if Vx != Vy then skip next instr */
            if(V[x] == V[y]) PC += 2;
            break;
        case 0x6:
            /* mvi / Vx := kk */
            V[x] = imm;
            break;
        case 0x7:
            /* addi / Vx += kk */
            V[x] += imm;
            break;
        case 0x8:
            /* dual reg ops */
            switch (instr_bus&0x000F) {
                case 0x0:
                    /* mv / Vx = Vy */
                    V[x] = V[y];
                    break;
                case 0x1:
                    /* or / Vx |= Vy */
                    V[x] |= V[y];
                    break;
                case 0x2:
                    /* and / Vx &= Vy */
                    V[x] &= V[y];
                    break;
                case 0x3:
                    /* xor / Vx ^= Vy */
                    V[x] ^= V[y];
                    break;
                case 0x4:
                    /* add / Vx += Vy */
                    V[x] += V[y];
                    V[0xF] = ((uint16_t)prev + (uint16_t)V[y]) > 255;
                    break;
                case 0x5:
                    /* sub / Vx -= Vy */
                    V[x] = V[x] - V[y];
                    V[0xF] = prev >= V[y];
                    break;
                case 0x6:
                    /* shr / Vx >>= Vy */
                    V[x] >>= 1; // V[y];
                    V[0xF] = 0b1&prev;
                    break;
                case 0x7:
                    /* subn / Vx =- Vy */
                    V[x] = V[y] - V[x];
                    V[0xF] = V[y] >= prev;
                    break;
                case 0xE:
                    /* shl / Vx <<= Vy */
                    V[x] <<= 1; // V[y];
                    V[0xF] = (0b10000000&prev) >> 7;
                    break;
            }
            break;
        case 0x9:
            /* sne / if Vx != Vy then skip next instr */
            if(V[x] != V[y]) PC += 2;
            break;
        case 0xA:
            /* ld / I := NNN */
            I = addr;
            break;
        case 0xB:
            /* jp / PC := NNN + V0 */
            PC = addr + V[0];
            break;
        case 0xC:
            /* rnd / Vx := rand & kk */
            V[x] = ((uint8_t)rand()) & imm;
            break;
        case 0xD:
            /* drw / draw sprite from I at (Vx, Vy)*/
            V[0xF] = 0;

            for(int i = 0; i < (instr_bus&0x000F); i++) {
                int index_x, index_y, offset;
                index_x = V[x]>>3;          // which byte to access
                index_y = (V[y] + i) % 32;  // height of byte
                offset = 8-(V[x]%8);        // offset of wrapped byte

                // Probably could speed this up
                if((screen[index_x][index_y] & (mem[I+i]>>(V[x]%8))) ||
                    (screen[(index_x+1)%8][index_y] & (mem[I+i]<<offset))) {
                        V[0xF] = 1;
                }

                screen[index_x][index_y] ^= (mem[I+i]>>(V[x]%8));
                screen[(index_x+1)%8][index_y] ^= (mem[I+i]<<offset);
            }
            break;
        case 0xE:
            if((instr_bus&0x00ff) == 0x9E) {
                /* skp / skip next instr if key Vx is pressed */
                if(key[V[x]]) PC += 2;
            } else if((instr_bus&0x00ff) == 0xA1) {
                /* sknp / skip next instr if key Vx is not pressed */
                if(!key[V[x]]) PC += 2;
            }
            break;
        case 0xF:
            switch (instr_bus&0x00FF) {
                case 0x07:
                    /* Vx := DT */
                    V[x] = DT;
                    break;
                case 0x0A:
                    /* Vx = key (block) */
                    int i;
                    for(i = 0; i < 16; i++) {
                        if(key[i]) break;
                    }
                    if(!key[i%16]) PC -= 2;
                    break;
                case 0x15:
                    /* DT := Vx */
                    DT = V[x];
                    break;
                case 0x18:
                    /* ST := Vx */
                    ST = V[x];
                    // play_beep();
                    break;
                case 0x1E:
                    /* I += Vx */
                    I += V[x];
                    break;
                case 0x29:
                    /* I := sprite[Vx] */
                    I = mem[x * 5];
                    break;
                case 0x33:
                    /* BCD representation of Vx in memory locations I, I+1, and I+2 */
                    //for(int i = 0; i < 3; i++) mem[I + 2 - i)] = (V[x] / (uint8_t)pow(10, i)) % 10;
                    mem[I+2] = V[x] % 10;
                    mem[I+1] = (V[x] / 10) % 10;
                    mem[I] = (V[x] / 100) % 10;
                    break;
                case 0x55:
                    /* I := { V0, ..., Vx } */
                    for(int i = 0; i <= x; i++) mem[I+i] = V[i];
                    break;
                case 0x65:
                    /* { V0, ..., Vx } := I */
                    for(int i = 0; i <= x; i++) V[i] = mem[I+i];
                    break;
            }
            break;
    }
}

/* * * * * * * * * * * * * * */


int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Include file name as arg.");
        return 1;
    } else if(argc > 2) {
        printf("Too many arguments.");
        return 1;
    }

    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 5; j++) {
            mem[i*5 + j] = sprite[i][j];
        }
    }

    int exit = read_rom_from_drive(mem, argv[1]);

    SDL_Window *window;
    SDL_Renderer *renderer;

    init_video(&window, &renderer);
    // init_audio();
    
    while(PC != exit) {
        execute(
            decode(
                fetch()
            )
        );
        SDL_Event event;
        if (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) break;
            if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) poll_keys(key, &event);
        }
        
        draw_screen(screen, renderer);

        struct timespec ts;
        ts.tv_nsec = 2000000 / 5;
        nanosleep(&ts, &ts);
    }

    close_video(window, renderer);
    // close_audio();
}