#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640,320);
    glutCreateWindow("Example 1 Display");

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}