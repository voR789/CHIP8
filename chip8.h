#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <string>

using std::string;

class chip8{
    private:
    uint8_t memory[4096];
    uint8_t V[16]; // 8 bit registers v0 - f
    uint16_t I; // Index register
    uint16_t PC; // Program counter
    uint16_t current_opcode;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t stack[16];
    uint8_t stack_ptr;
    uint8_t gfx[2048]; // 64*32
    uint8_t key[16];
public:    
    bool drawFlag;
    void initialize();
    void emulateCycle();
    void loadGame(string name);
    void setKeys();
};

#endif