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
double lineTheta = 0;
double frame = 0;

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

void drawCircle(double radius, int segments)
{
    double prevx = radius, prevy = 0;
    double theta = 0;

    glBegin(GL_LINES);
    for(int i = 0; i <= segments; i++)
    {
        double x = radius * cosf(theta);
        double y = radius * sinf(theta);
        glVertex2f(prevx, prevy);
        glVertex2f(x, y);
        prevx = x;
        prevy = y;
        theta += (2 * M_PI) / segments;
    }
    glEnd();
}

void drawMiniCircle(double bigCircleRadius, double miniCircleRadius, int segments)
{
    double lineThetaRad = lineTheta * M_PI / 180;
    double x = bigCircleRadius * cosf(lineThetaRad);
    double y = bigCircleRadius * sinf(lineThetaRad);

    glPushMatrix();
    glTranslatef(x, y, 0);
    drawCircle(miniCircleRadius, segments);
    glPopMatrix();
}

void drawRotatingLine(double bigCircleRadius)
{
    double lineThetaRad = lineTheta * M_PI / 180;
    double x = bigCircleRadius * cosf(lineThetaRad);
    double y = bigCircleRadius * sinf(lineThetaRad);

    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(x, y);
    glEnd();
}

void drawReferenceLine(double bigCircleRadius) {
    double lineThetaRad = lineTheta * M_PI / 180;
    double x = bigCircleRadius * cosf(lineThetaRad);
    double y = bigCircleRadius * sinf(lineThetaRad);

    double fixedX = bigCircleRadius + 0.15;
    double nonFixedY = y;

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(fixedX, nonFixedY);
    glEnd();

} 

double arr[1000];
int ind = 0;

void drawSine(double radius)
{
    glColor3f(1,1,1);

    arr[ind] = radius * sin(M_PI / 180 * lineTheta);;

    double xx = radius + 0.15;
    for(int i=ind;i>=0;i--)
    {
        glBegin(GL_POINTS);
        glVertex2f(xx,arr[i]);
        glEnd();

        xx += 0.002;
    }
}

void display(){
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glMatrixMode(GL_MODELVIEW);     
    // glLoadIdentity();

    // gluLookAt(pos.x, pos.y, pos.z,	pos.x + lookat.x, pos.y + lookat.y, pos.z + lookat.z, up.x, up.y, up.z);
    // gluLookAt(pos.x, pos.y, pos.z,	lookat.x, lookat.y, lookat.z, up.x, up.y, up.z);
    // drawAxes();

    double bigCircleRadius = 0.5;

    glPushMatrix();
    glTranslatef(-0.35, 0, 0);

    glColor3f(0, 0, 1);
    drawCircle(bigCircleRadius, 100);
    glColor3f(1, 0, 0);
    drawMiniCircle(bigCircleRadius, 0.015, 40);
    glColor3f(1, 1, 1);
    drawRotatingLine(bigCircleRadius);
    drawReferenceLine(bigCircleRadius);
    drawSine(bigCircleRadius);

    ind++;

    glPopMatrix();

    // glFlush();
    glutSwapBuffers();
}

void idle(){
    glutPostRedisplay();
}

void timer(int){
    lineTheta = lineTheta + 1;
    if (lineTheta >= 360) {
        lineTheta = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    // glEnable(GL_DEPTH_TEST);

    frame = 0.5 + 0.15;
    
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    // gluPerspective(40, 1, 1, 1000.0);
    
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitWindowSize(850, 850);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
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