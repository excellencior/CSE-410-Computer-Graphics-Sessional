#include<bits/stdc++.h>
#include<math.h>

#ifdef __linux__
    #include<GL/glut.h>
#elif WIN32
    #include <GL/windows.h>
    #include <glut.h>
#endif

void keyboardListener(unsigned char, int, int);
void specialKeyListener(int , int , int);

double SCALE_FACTOR = 0.5;
//  ------------------------------------ x --------------------------------------

struct PT {
    double x, y, z;

    PT() {
        // Required for non-parameterized object creation
    }

    PT(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // Copy constructor
    PT(const PT &p) {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
    }

    PT& operator = (const PT &ob2) {
        this->x = ob2.x;
        this->y = ob2.y;
        this->z = ob2.z;
        return *this;
    }
};

// Operator Overloading functions
PT operator + (const PT &ob1, const PT &ob2) {
    return PT(ob1.x + ob2.x, ob1.y + ob2.y, ob1.z + ob2.z);
}

PT operator - (const PT &ob1, const PT &ob2) {
    return PT(ob1.x - ob2.x, ob1.y - ob2.y, ob1.z - ob2.z);
}

// Cross product
PT operator * (const PT &ob1, const PT &ob2) {
    return PT( (ob1.y * ob2.z) - (ob2.y * ob1.z), (ob2.x * ob1.z) - (ob1.x * ob2.z), (ob1.x * ob2.y) - (ob2.x * ob1.y) );
}

// vector * const
PT operator * (const PT &ob, const double num) {
    return PT(ob.x * num, ob.y * num, ob.z * num);
}

// vector / const
// vector * const
PT operator / (const PT &ob, const double num) {
    return PT(ob.x*1.0 / num, ob.y*1.0 / num, ob.z*1.0 / num);
}

std::ostream& operator<<(std::ostream& os, const PT& ob) {
    os << "x = " << ob.x << ", y = " << ob.y << ", z = " << ob.z << std::endl;
    return os;
}

PT pos(5, 0, 0);

PT up(0,0,1);
PT rightside(0,1,0);
PT lookat(-1,0,0);
double startX;
bool toggleY;
double angle = 0;
double theta = 0;

using namespace std;

void drawAxes()
{
    glBegin(GL_LINES);{
        glColor3f(1.0, 0.0, 0.0); // X axis
        glVertex3f( 20,0,0);
        glVertex3f(-20,0,0);

        glColor3f(0.0, 1.0, 0.0); // Y axis
        glVertex3f(0,-20,0);
        glVertex3f(0, 20,0);

        glColor3f(0.0, 0.0, 1.0); // Z axis
        glVertex3f(0,0, 20);
        glVertex3f(0,0,-20);
    }glEnd();
}

void drawSolidCircle(double radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, 0);
    }
    glEnd();
}

void drawEllipse(double a, double b, int segments) {
    glBegin(GL_LINES);
    for (int i = 0; i < segments; ++i) {
        double angle1 = 2.0 * M_PI * i / segments;
        double angle2 = 2.0 * M_PI * (i + 1) / segments;
        
        double x1 = a * cos(angle1);
        double y1 = b * sin(angle1);
        double x2 = a * cos(angle2);
        double y2 = b * sin(angle2);

        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
    }
    glEnd();
}

void drawSolidCircleMoon(double bigRadius, double radius, int segments) {
    glPushMatrix();
    glBegin(GL_TRIANGLE_FAN);

    bigRadius = bigRadius * 2.5;
    double centerX = bigRadius * cos(angle * M_PI / 180.0); // Calculate x-coordinate of the center of the solid circle
    double centerY = bigRadius * sin(angle * M_PI / 180.0); // Calculate y-coordinate of the center of the solid circle

    for (int i = 0; i <= segments; ++i) {
        double angleMoon = angle + (360.0 * i / segments); // Angle for the moon circle
        double moonX = centerX + radius * cos(angleMoon * M_PI / 180.0); // Calculate x-coordinate of the moon circle
        double moonY = centerY + radius * sin(angleMoon * M_PI / 180.0); // Calculate y-coordinate of the moon circle
        glVertex3f(moonX, moonY, 0);
    }

    glEnd();
    glPopMatrix();
}


void drawRotatingSolidCircle(double a, double b) {
    glPushMatrix();
    double x = a * cos(angle * M_PI / 180.0); // Calculate x-coordinate
    double y = b * sin(angle * M_PI / 180.0); // Calculate y-coordinate
    glTranslatef(x, y, 0); // Translate to the calculated position
    // drawSolidCircle(0.05, 40); // Draw the solid circle

    drawSolidCircleMoon(0.05, 0.025, 20);

    glPopMatrix();
}

void display(){
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glMatrixMode(GL_MODELVIEW);     
    // glLoadIdentity();

    // gluLookAt(pos.x, pos.y, pos.z,	pos.x + lookat.x, pos.y + lookat.y, pos.z + lookat.z, up.x, up.y, up.z);
    // gluLookAt(pos.x, pos.y, pos.z,	lookat.x, lookat.y, lookat.z, up.x, up.y, up.z);
    // drawAxes();

    glColor3f(1.0, 1.0, 1.0);
    drawEllipse(0.5, 0.4, 100);
    glColor3f(1.0, 0.0, 0.0);
    drawRotatingSolidCircle(0.5, 0.4);


    // glFlush();
    glutSwapBuffers();
}

void idle(){
    glutPostRedisplay();
}

void timer(int){
    double speedFactor = 4.0; // Speed factor for rotation

    angle += speedFactor; // Increment the angle of rotation

    if (angle >= 360.0)
        angle -= 360.0; // Keep the angle within the range [0, 360)


    theta += 10;
    if (theta >= 360.0)
        theta = 0;

    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);
}


void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    // glEnable(GL_DEPTH_TEST);

    startX = -0.5;
    toggleY = true;
    
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    // gluPerspective(40, 1, 1, 1000.0);
    
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitWindowSize(750, 750);   // Set the window's initial width & height
    glutInitWindowPosition(750, 250); // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Test");

    glutDisplayFunc(display);
    // glutKeyboardFunc(keyboardListener);
    // glutSpecialFunc(specialKeyListener);
    // glutIdleFunc(idle);
    glutTimerFunc(10, timer, 0);

    init();

    glutMainLoop();
    return 0;

}

void specialKeyListener(int key, int x, int y){
    switch (key)
    {
    case GLUT_KEY_UP:
        pos = pos + (lookat * SCALE_FACTOR);
        break;

    case GLUT_KEY_DOWN:
        pos = pos - (lookat * SCALE_FACTOR);
        break;

    case GLUT_KEY_LEFT:
        pos = pos - (rightside * SCALE_FACTOR);
        break;

    case GLUT_KEY_RIGHT:
        pos = pos + (rightside * SCALE_FACTOR);
        break; 
    
    case GLUT_KEY_PAGE_UP:
        pos = pos + (up * SCALE_FACTOR);
        break;

    case GLUT_KEY_PAGE_DOWN:
        pos = pos - (up * SCALE_FACTOR);
        break;
    
    default:
        printf("We don't know what you pressed: %c\n", key);
        break;
    }
}

void keyboardListener(unsigned char key, int x, int y) {
    switch(key) {
        // case '.':
        //     if (triangleCornerDistance < maximumTriangleCornerDistance) {
        //         triangleCornerDistance += perClickChange;
        //         sphereRadius -= (maximumSphereRadius * perClickChange);
        //     }
        //     break;

        default:
            cout << "Invalid Key Pressed" << endl;
            break;
    }
}