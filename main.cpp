#include "chip8.h"
#include <iostream>
#include <GL/glut.h>

chip8 cpu;

// Graphics
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    uint8_t* gfx = cpu.getGFX();
    glColor3f(1.0,1.0,1.0); // set draw color to white
    // drawing loop
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 64; j++){
            if(gfx[i*64+j]){
                // pixel drawing
                glBegin(GL_QUADS);
                    glVertex2i(j,i);
                    glVertex2i(j+1,i);
                    glVertex2i(j + 1,i + 1);
                    glVertex2i(j,i + 1);
                glEnd();
            }
        }
    }
    glFlush();
}

void loadGraphics(int argc, char** argv){
    glutInit(&argc, argv);
    // window setup
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(64*20,32*20); // x20 resolution
    glutCreateWindow("Chip8 System");
    glutDisplayFunc(display);

    // remap to more useful coordinates
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 64.0, 32.0, 0.0, -1.0, 1.0); // Left, Right, Bottom, Top
    glMatrixMode(GL_MODELVIEW);
}

// Input
void keyPressHandler(unsigned char keyPress, int x, int y){
    uint8_t* keyPtr = cpu.getKEY();
    switch(toupper(keyPress)){
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
    switch(toupper(keyPress)){
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

void timer(int value){
    cpu.emulateCycle();
    cpu.emulateCycle();
    cpu.emulateCycle();
    cpu.emulateCycle();
    
    if(cpu.drawFlag){
        glutPostRedisplay();
        cpu.drawFlag = false;
    }

    glutTimerFunc(16, timer, 0);
}
int main(int argc, char** argv){
    int select;
    std::cout << "Select [Type 1, 2, 3] a game from the options: " << '\n' << "Pong [1]" << '\n' << "Breakout [2]" << '\n' << "Invaders [3]" << '\n';
    std::cin >> select;
    
    // Initialize CPU, and load game
    cpu.initialize();
    switch(select){
        case(1):
            std::cout << '\n' << "Controls:" << '\n' << "Left Paddle: [Q] [1]" << '\n' << "Right Paddle [4] [R]" << std::endl;
            cpu.loadGame("Pong.ch8"); 
            break;
        case(2):
            std::cout << '\n' << "Controls:" << '\n' << "Paddle: [Q] [E]" << std::endl;
            cpu.loadGame("Breakout.ch8"); 
            break;
        case(3):
            std::cout << '\n' << "Controls:" << '\n' << "Spaceship: [Q] [E]" << std::endl;
            cpu.loadGame("Invaders.ch8"); 
            break;
    }
    
    // Setup graphics and input
    loadGraphics(argc,argv);
    loadInput();//
    glutTimerFunc(0, timer, 0);
    // Emulation loop using GLUT
    glutMainLoop();
    return 0;
}
// run executable with ./chip8