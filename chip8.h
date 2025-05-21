#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <string>

using std::string;

class chip8{
    private:
    uint8_t memory[4096];
    uint8_t V[16]; // 8 bit registers v0 - f, used for operations
    uint16_t I; // Index register, holds a useful index in memory
    uint16_t PC; // Program counter - helps the interpreter trace the program file.
    uint16_t current_opcode;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t stack[16];
    uint8_t stack_ptr;
    uint8_t gfx[2048]; // 64*32
    uint8_t key[16];
public:   
    uint8_t* getGFX();
    uint8_t* getKEY();
    
    bool drawFlag;
    void initialize();
    void emulateCycle();
    void loadGame(string name);
    void setKeys();

    

};

#endif