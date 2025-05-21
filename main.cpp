#include "chip8.h"
#include <GL/glut.h>

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

chip8 cpu;
int main(int argc, char** argv){
    // Setup graphics and input
    loadGraphics(argc,argv);
    // Initialize CPU, and load game
    cpu.initialize();

    //cpu.loadGame(""); 
    // Emulation loop

    for(;;){
    
    }
    

    return 0;
}
// run executable with ./chip8