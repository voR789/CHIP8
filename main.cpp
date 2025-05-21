#include "chip8.h"
#include <GL/glut.h>

chip8 cpu;

// Graphics
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void loadGraphics(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(64*20,32*20); // x20 resolution
    glutCreateWindow("Chip8 System");
    glutDisplayFunc(display);
    glutMainLoop();
}

// Input
void keyPressHandler(unsigned char keyPress, int x, int y){
    uint8_t* keyPtr = cpu.getKEY();
    switch(keyPress){
        case('1'):{keyPtr[0x1] = 1; break;}
        case('2'):{keyPtr[0x2] = 1; break;}
        case('3'):{keyPtr[0x3] = 1; break;}
        case('4'):{keyPtr[0xC] = 1; break;}
        case('Q'):{keyPtr[0x4] = 1; break;}
        case('W'):{keyPtr[0x5] = 1; break;}
        case('E'):{keyPtr[0x6] = 1; break;}
        case('R'):{keyPtr[0xD] = 1; break;}
        case('A'):{keyPtr[0x7] = 1; break;}
        case('S'):{keyPtr[0x8] = 1; break;}
        case('D'):{keyPtr[0x9] = 1; break;}
        case('F'):{keyPtr[0xE] = 1; break;}
        case('Z'):{keyPtr[0xA] = 1; break;}
        case('X'):{keyPtr[0x0] = 1; break;}
        case('C'):{keyPtr[0xB] = 1; break;}
        case('V'):{keyPtr[0xF] = 1; break;}
        // following suggested keymap from Laurence Muller's overview
    }
}

void keyReleaseHandler(unsigned char keyPress, int x, int y){
    uint8_t* keyPtr = cpu.getKEY();
    switch(keyPress){
        case('1'):{keyPtr[0x1] = 0; break;}
        case('2'):{keyPtr[0x2] = 0; break;}
        case('3'):{keyPtr[0x3] = 0; break;}
        case('4'):{keyPtr[0xC] = 0; break;}
        case('Q'):{keyPtr[0x4] = 0; break;}
        case('W'):{keyPtr[0x5] = 0; break;}
        case('E'):{keyPtr[0x6] = 0; break;}
        case('R'):{keyPtr[0xD] = 0; break;}
        case('A'):{keyPtr[0x7] = 0; break;}
        case('S'):{keyPtr[0x8] = 0; break;}
        case('D'):{keyPtr[0x9] = 0; break;}
        case('F'):{keyPtr[0xE] = 0; break;}
        case('Z'):{keyPtr[0xA] = 0; break;}
        case('X'):{keyPtr[0x0] = 0; break;}
        case('C'):{keyPtr[0xB] = 0; break;}
        case('V'):{keyPtr[0xF] = 0; break;}
    }
}

void loadInput(){
    glutKeyboardFunc(keyPressHandler);
    glutKeyboardUpFunc(keyReleaseHandler);
}

int main(int argc, char** argv){
    // Setup graphics and input
    loadGraphics(argc,argv);
    loadInput();
    // Initialize CPU, and load game
    cpu.initialize();

    //cpu.loadGame(""); 
    // Emulation loop

    for(;;){
    
    }
    

    return 0;
}
// run executable with ./chip8