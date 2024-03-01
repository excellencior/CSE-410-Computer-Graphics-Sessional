#include<bits/stdc++.h>
#include<math.h>

#ifdef __linux__
    #include<GL/glut.h>
#elif WIN32
    #include <windows.h>
    #include <GL/glut.h>
#endif

void keyboardListener(unsigned char, int, int);
void specialKeyListener(int , int , int);

double cubeRotationAngle = 0;
bool animateLeft = false;
double rotAmount = 0, rotSpeed = 1.0;

using namespace std;

void drawAxes()
{
    glBegin(GL_LINES);{
        glColor3f(1.0, 1.0, 1.0); // X axis
        glVertex3f(4,0,0);
        glVertex3f(-4,0,0);

        glVertex3f(0,-4,0);
        glVertex3f(0, 4,0);

        glVertex3f(0,0, 4);
        glVertex3f(0,0,-4);
    }glEnd();
}

void drawCircle(double radius, int segments)
{
    double prevx = radius, prevy = 0;
    double theta = 0;

    glBegin(GL_LINES);
    for(int i = 0; i <= segments; i++)
    {
        double x = radius * cosf(theta);
        double y = radius * sinf(theta);
        glVertex3f(prevx, prevy, 0);
        glVertex3f(x, y, 0);
        prevx = x;
        prevy = y;
        theta += (2 * M_PI) / segments;
    }
    glEnd();
}

void drawSolidRectangle(double a) {
    glBegin(GL_QUADS);
    glVertex3f(a, a, 0);
    glVertex3f(-a, a, 0);
    glVertex3f(-a, -a, 0);
    glVertex3f(a, -a, 0);
    glEnd();
}

void drawSlates() {

    for (int i = 0; i < 6; i++) {
        glPushMatrix();

        glRotatef(i * 60, 0, 0, 1);
        glTranslatef(1, 0, 0);
        glRotatef(rotAmount, 0, -1, 0);
        glTranslatef(0, 0, -1);

        drawSolidRectangle(0.125);
        glPopMatrix();
    }

    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glRotatef(i * 60, 0, 0, 1);
        glTranslatef(1, 0, 0);
        glRotatef(rotAmount, 0, -1, 0);

        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0.125, -1);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, -0.125, -1);
        glEnd();
        
        glPopMatrix();
    }
}

void display(){
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     
    glLoadIdentity();

    gluLookAt(5, 5, 5,	0, 0, 0, 0, 0, 1);

    glPushMatrix();
    glRotatef(2 * cubeRotationAngle, 0, 0, 1);

    drawAxes();
    drawCircle(1, 100);
    drawSlates();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void idle(){
    if (rotAmount > 0) {
        cubeRotationAngle -= 0.1 * rotSpeed;
        if (cubeRotationAngle < 0)
            cubeRotationAngle = 360;
    }

    glutPostRedisplay();
}

void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40, 1, 1, 1000.0);
    
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitWindowSize(750, 750);   // Set the window's initial width & height
    glutInitWindowPosition(750, 250); // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Test");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
    return 0;

}

void specialKeyListener(int key, int x, int y){
    switch (key)
    {
    case GLUT_KEY_LEFT:
        cubeRotationAngle -= 1;
        if (cubeRotationAngle < 0)
            cubeRotationAngle = 360;
        break;

    case GLUT_KEY_RIGHT:
        cubeRotationAngle += 1;
        if (cubeRotationAngle > 360)
            cubeRotationAngle = 0;
        break;
    
    default:
        printf("We don't know what you pressed: %c\n", key);
        break;
    }
}

void keyboardListener(unsigned char key, int x, int y) {
    switch(key) {
        case '1':

            rotAmount += 5;
            rotSpeed += 0.1;
            if (rotAmount >= 90)
                rotAmount = 90;
            break;

        case '2':
            rotAmount -= 5;
            rotSpeed -= 0.1;
            if (rotAmount <= 0)
                rotAmount = 0;
            break;

        default:
            cout << "Invalid Key Pressed" << endl;
            break;
    }
}