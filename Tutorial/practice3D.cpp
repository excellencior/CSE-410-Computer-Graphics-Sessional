#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <GL/glut.h>
#endif

void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);
}

void drawAxes() {
    // Draw line between 2 glVertex2f calls
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-100, 0.0f, 0);
        glVertex3f(100, 0.0f, 0);

        glVertex3f(0.0f, 100, 0);
        glVertex3f(0.0f, -100, 0);

        glVertex3f(0.0f, 0.0f, 100);
        glVertex3f(0.0f, 0.0f, -100);
    }
    glEnd();
}

int cnt = 1;

void drawSquare(double armLen) {
    glBegin(GL_QUADS);{
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-armLen/2, -armLen/2, 0);
        glVertex3f(armLen/2, -armLen/2, 0);
        glVertex3f(armLen/2, armLen/2, 0);
        glVertex3f(-armLen/2, armLen/2, 0);
    }glEnd(); 
}


void display() {
    printf("Repainting.. %d\n", cnt);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(50, 50, 50, 0, 0, 0, 0, 1, 0);

    drawAxes();

    // drawRectangles(.5);
    // drawRectanglesWithRotation(.5);
    drawSquare(.5);

    glFlush();
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(500, 500);
    // glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("OpenGL Demo");

    init();

    glutDisplayFunc(display);
    // Call the idle function when openGL is not computing anything (idle)
    // glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}