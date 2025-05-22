#include <string>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <GL/glut.h>
#include "chip8.h"

using std::string, std::memset, std::ifstream, std::cerr;

// Helpers
uint8_t* chip8::getGFX(){
    return gfx;
}
uint8_t* chip8::getKEY(){
    return key;
}

// Game
void chip8::initialize(){
    PC = 0x200; // set program counter to first address of actual interpreter
    current_opcode = 0;
    I = 0;
    stack_ptr = 0;
    sound_timer = 0;
    delay_timer = 0;

    // clear all arrays w/ memset
    memset(V,0,sizeof(V)); //  0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    memset(stack,0,sizeof(stack));
    memset(memory,0,sizeof(memory));
    memset(gfx,0,sizeof(gfx));

    // load fonts
    u_int8_t fonts[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
    for(int i = 0; i < 80; i++){ // start at 80, because = to 0x50 in hex
        memory[i] = fonts[i];
    }    
}

void chip8::loadGame(string name){
    ifstream file(name, std::ios::binary);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open ROM file");
    }
    uint8_t byte;
    size_t i = 0;
    while(file.read(reinterpret_cast<char*>(&byte), 1)){ // read byte by byte
        memory[512 + i++] = byte;
    }
    file.close(); 
}

void chip8::emulateCycle(){
    current_opcode = memory[PC] << 8 | memory[PC + 1]; // fetch two byte opcode out of byte array
    // 0xNNNN

    uint8_t first = (current_opcode & 0xF000) >> 12;
    uint8_t second = (current_opcode & 0x0F00) >> 8;
    uint8_t third = (current_opcode & 0x00F0) >> 4;
    uint8_t fourth = (current_opcode & 0x000F);

    switch (first)
    {
    case(0x0):  
        switch(fourth){
            case(0x0):
                // Clear the screen
                memset(gfx,0,sizeof(gfx));
                PC += 2;
                break;
            case(0xE):
                // Return from subroutine
                stack_ptr--; // "pop" the stack
                PC = stack[stack_ptr]; // return to stored position
                break;
            default:
                throw std::runtime_error("Unknown 0x0 opcode");
        }
        break;
    case(0x1): // (0x1NNN)
        // Jump to address NNN - permanent jump
        PC = (current_opcode & 0x0FFF);
        break;
    case(0x2): // (0x2NNN)
        // Calls subroutine at NNN - return later, so store PC initial in stack
        stack[stack_ptr] = PC + 2;
        stack_ptr++;
        PC = (current_opcode & 0x0FFF);
        break;
    // Key - hex code 0-F can be equal to binary 0-15
    case(0x3): // (3XNN)
        // Skips the next instruction if Vx = NN
        if(V[second] == (third << 4 | fourth)){
            PC += 4;
        } else{
            PC += 2;
        }
        break;
    case(0x4): // (4XNN)
        // Skips the next instruction if Vx != NN
        if(V[second] != (third << 4 | fourth)){
            PC += 4;
        } else{
            PC += 2;
        }
        break;
    case(0x5): // (5XY0)
        // Skips to next instruction if Vx = Vy
        if(V[second] == V[third]){
            PC += 4;
        } else{
            PC += 2;
        }
        break;
    case(0x6): // (6XNN)
        // Sets Vx to NN
        V[second] = (third << 4 | fourth);
        PC += 2;
        break;
    case(0x7): // (7XNN) 
        // Adds NN to VX - do not change carry flag
        V[second] += (third << 4 | fourth);
        PC += 2;
        break;
    case(0x8): // (8XY.)
        switch(fourth){
            case(0x0): // (8XY0)
                // Sets Vx to the value of Vy
                V[second] = V[third];
                PC += 2;
                break;
            case(0x1): // (8XY1)
                // Sets Vx to Vx | Vy
                V[second] = (V[second] | V[third]);
                PC += 2;
                break;
            case(0x2): // (8XY2)
                // Sets Vx to Vx & Vy
                V[second] = (V[second] & V[third]);
                PC += 2;
                break;
            case(0x3): // (8XY3)
                // Sets Vx to Vx ^ Vy
                V[second] = (V[second] ^ V[third]);
                PC += 2;
                break;
            case(0x4):{ // (8XY4)
                // Vx = Vx + Vy, with carry flag (Vf)
                int sum = V[second] + V[third];
                if(sum > 255){
                    V[0xF] = 1;
                    V[second] = sum & 0xFF;
                } else {
                    V[0xF] = 0;
                    V[second] = sum;
                }
                PC += 2;
                break;
            }
            case(0x5):{ // (8XY5)
                // Vx = Vx - Vy, with inverse carry flag (Vf)
                int diff = V[second] - V[third];
                if(diff < 0){
                    V[0xF] = 0;
                    V[second] = diff & 0xFF;
                } else {
                    V[0xF] = 1;
                    V[second] = diff;
                }
                PC += 2;
                break;
            }
            case(0x6): // (8XY6)
                // Shifts Vx right by 1, stores least sig bit of Vx prior in Vf
                V[0xF] = V[second] & 0x1;
                V[second] = V[second] >> 1;
                PC += 2;
                break;
            case(0x7):{ // (8XY7)
                // Vx = Vy - Vx, with carry flag (Vf)
                int diff = V[third] - V[second];
                if(diff < 0){
                    V[0xF] = 0;
                    V[second] = diff & 0xFF;
                } else {
                    V[0xF] = 1;
                    V[second] = diff;
                }
                PC += 2;
                break;
            }
            case(0xE): // (8XYE)
                // Shifts Vx to the left by 1, sets Vf to 1 if most significant digit of Vx was set, 0 if not
                V[0xF] = V[second] >> 7;
                V[second] = V[second] << 1;
                PC += 2;
                break;
            default:
                throw std::runtime_error("Unknown 0x8__ opcode");
        }
        break;
    case(0x9): // (9XY0) 
        // Skips next instruction if Vx != Vy
        if(V[second] != V[third]){
            PC += 4;
        } else {
            PC += 2;
        }
        break;
    case(0xA): // (ANNN)
        // Sets I to address NNN
        I = (second << 8 | third << 4) | fourth;
        PC += 2;
        break;
    case(0xB): // (BNNN) 
        // Jumps to the address NNN + V0
        PC = ((second << 8 | third << 4) | fourth) + V[0];
        break;
    case(0xC): // (CXNN) 
        // Sets Vx to the result of rand(0,255) & NN
        V[second] = (third << 4 | fourth) & (rand() % 256);
        PC += 2;
        break;
    case(0xD):{ // (DXYN) 
        // Draws a sprite at (Vx,Vy) with a width 8 and height N px. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
        V[0xF] = 0;
        int index;
        int pixel;

        for(int i = 0; i < fourth; i++){ // loop through rows to N
            uint8_t sprite_line = memory[I + i];
            for(int j = 0; j < 8; j++){
                index = ((V[third]+i) % 32)*64 + (V[second] + j) % 64; 
                pixel = (sprite_line  >> (7-j) & 0x1); // bitmask to isolate digits (1000,0000)
                if(gfx[index] == 1 && pixel){ // if gfx[index] = 1, and if initial pixel was one, then that means a collision happened
                    V[0xF] = 1;
                }
                gfx[index] ^= pixel;

            }
        }
        PC += 2;
        break;
        }
    case(0xE): // (EX..)
        switch(third){
            case(0x9): // (EX9E)
                // If the key pressed = Vx, skip the next instruction
                if(key[V[second]]){
                    PC += 4;
                } else{
                    PC += 2;
                }
                break;
            case(0xA): // (EXA1)
                // If the key pressed != Vx, skip the next instruction 
                if(!key[V[second]]){
                    PC += 4;
                } else{
                    PC += 2;
                } 
                break;
            default:
                throw std::runtime_error("Unknown 0xE__ opcode");
        }
        break;
    case(0xF): // (FX..)
        switch(third){
            case(0x0): // (FX0.)
                switch(fourth){
                    case(0x7): // (FX07)
                        // Sets Vx to delay timer
                        V[second] = delay_timer;
                        break;
                    case(0xA):{ // (FX0A)
                        // A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing)
                        for(int i = 0; i < 16; i++){
                                if(key[i] == 1){
                                    V[second] = i;
                                    PC += 2; // since emulate cycle is called every "tick", this will loop until a key is pressed
                                    break;
                                }
                        }
                        break;
                    }
                    default:
                        throw std::runtime_error("Unknown 0xF0_ opcode");
                }
                break;
            case(0x1): // (FX1.)
                switch(fourth){
                    case(0x5): // (FX15)
                        // Sets delay timer to Vx
                        delay_timer = V[second];
                        break;
                    case(0x8): // (FX18)
                        // Sets sound timer to Vx
                        sound_timer = V[second];
                        break;
                    case(0xE): // (FX1E)
                        // Adds Vx to I, no carry flag (Vf)
                        I += V[second];
                        break;
                    default:
                        throw std::runtime_error("Unknown 0xF1_ opcode");
                }
                break;
            case(0x3):{ // (FX33)    
                // Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
                unsigned int num = V[second];
                // 1234
                memory[I] = (num/100)%10;
                memory[I+1] = (num/10)%10;
                memory[I+2] = (num)%10; 
                PC += 2;
                break;
            }
            case(0x5):{ // (FX55)
                // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                for(int i = 0; i <= second; i++){
                    memory[I+i] = V[i];
                }
                PC += 2;
                break;
            }
            case(0x6):{ // (FX65)
                // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
                for(int i = 0; i <= second; i++){
                    V[i] = memory[I+i];
                }
                PC += 2;
                break;
            }
        }
        break;
    default:
        throw std::runtime_error("Unknown opcode");
    }

    /*switch(current_opcode & 0xF000){ // grabs the head of the opcode
        // opcodes
        case 0x0000:

        
        default:
            throw std::runtime_error("Unknown opcode");
    }*/

    if(delay_timer > 0){
        --delay_timer;
    }
    if(sound_timer > 0){
        if(sound_timer == 1){
            // make noise
        }
        --sound_timer;
    }
}
