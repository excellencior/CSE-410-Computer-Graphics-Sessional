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
}

void drawAxes() {
    // Draw line between 2 glVertex2f calls
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);

        glVertex2f(0.0f, 1.0f);
        glVertex2f(0.0f, -1.0f);
    }
    glEnd();
}

void drawRectangles(double armLen) {
    double x[] = {.5f, -.5f, -.5f, .5f};
    double y[] = {.5f, .5f, -.5f, -.5f};

    for (int i=0; i<4; i++) {
        glPushMatrix();
        glTranslatef(x[i], y[i], 0.0f);
        glRotatef(45, 0, 0, 1);
        glBegin(GL_QUADS);{
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(-armLen/2, -armLen/2);
            glVertex2f(armLen/2, -armLen/2);
            glVertex2f(armLen/2, armLen/2);
            glVertex2f(-armLen/2, armLen/2);
        }glEnd();
        glPopMatrix();
        // glTranslatef(-x[i], -y[i], 0.0f);
    }
    
}

void drawRectanglesWithRotation(double armLen) {
    for (int i=0; i<4; i++) {
        glPushMatrix();
        // 1st rotation then origin translation
        glRotatef(90 * i, 0, 0, 1);
        glTranslatef(.5, .5, 0.0f);
        glBegin(GL_QUADS);{
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(-armLen/2, -armLen/2);
            glVertex2f(armLen/2, -armLen/2);
            glVertex2f(armLen/2, armLen/2);
            glVertex2f(-armLen/2, armLen/2);
        }glEnd(); 
        glPopMatrix();
        // glTranslatef(-x[i], -y[i], 0.0f);
    }
}

int cnt = 1;

void drawSquare(double armLen) {
    glPushMatrix();
    // 1st rotation then origin translation
    glRotatef(cnt % 360, 0, 0, 1); cnt++;
    glTranslatef(.5, .5, 0.0f);
    glBegin(GL_QUADS);{
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex2f(-armLen/2, -armLen/2);
        glVertex2f(armLen/2, -armLen/2);
        glVertex2f(armLen/2, armLen/2);
        glVertex2f(-armLen/2, armLen/2);
    }glEnd(); 
    glPopMatrix();
}



void display() {
    printf("Repainting.. %d\n", cnt);
    glClear(GL_COLOR_BUFFER_BIT);

    drawAxes();

    // drawRectangles(.5);
    // drawRectanglesWithRotation(.5);
    drawSquare(.5);

    glFlush();
}

void idle() {
    glutPostRedisplay();
}

void timerFunc(int value) {
    glutPostRedisplay();
    glutTimerFunc(10, timerFunc, 0);
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
    glutTimerFunc(1000, timerFunc, 0);
    
    glutMainLoop();
    return 0;
}