#include<bits/stdc++.h>
#include<math.h>
#include <ctime>
using namespace std;

#ifdef __linux__
    #include<GL/glut.h>
#elif WIN32
    #include <GL/windows.h>
    #include <glut.h>
#endif

const double rotation_angle = M_PI/ 180;
#define SCALE_FACTOR 0.4
// Camera at X
#define Y_COLLISION 2
#define X_COLLISION 1
int collisionDirection;

int manualMode = 1;
double wallLength, wallHeight;
double startTime, endTime, timeToCollide;

// Variable declaration section
int segmentCount;
double sectorStep, stackStep;
double sectorAngle, stackAngle;

double ballRadius;

double perClickChange;
double perClickroll = M_PI * 40 / 180;

// Arrow angle with Y
double arrowAngle;
double perClickArrowAngleChange;

double ballColor = 1.0f;

//  ---------------------------------------------------- x --------------------------------------
double quanta; // Time segment for the roll ball simulation 
// ======================================================================

class PT {
public:
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
PT operator / (const PT &ob, const double num) {
    return PT(ob.x*1.0 / num, ob.y*1.0 / num, ob.z*1.0 / num);
}

std::ostream& operator<<(std::ostream& os, const PT& ob) {
    os << "x = " << ob.x << ", y = " << ob.y << ", z = " << ob.z << std::endl;
    return os;
}

double dotProduct(PT v1, PT v2) {
    double res = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    return res;
}

PT pos(10, 0, 0);

PT up(0,0,1);
PT rightside(0,1,0);
PT lookat(-1,0,0);

PT ballCenter; 
PT ballVelocity;

// Saving the points of the ball surface
PT points[1001][1001];

// Function Prototypes
void specialKeyListener(int, int, int);
void keyboardListener(unsigned char, int, int);

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

void drawCheckerboard() {
    int size = 50;
    double colors = 1.0f;
    glPushMatrix();
    glTranslatef(0, 0, -ballRadius);
    for (int i= -size; i<=size; i+=1) {

        for (int j = -size; j<=size; j+=1) {
            glPushMatrix();
            colors = 1 - colors;
            glColor3f(colors, colors, colors);

            glBegin(GL_QUADS); {
                glVertex3f(i, j, 0);
                glVertex3f(i + 1, j, 0);
                glVertex3f(i + 1, j + 1, 0);
                glVertex3f(i, j + 1, 0);
            } glEnd();

            glPopMatrix();
        }
    }
    glPopMatrix();
}

double modulusOf(struct PT p) {
    double sum = (p.x * p.x) + (p.y * p.y) + (p.z * p.z);
    return sqrt(sum);
}

PT getUnitVectorOf(struct PT p) {
    double lengthInv = 1.0f / modulusOf(p);

    struct PT np;
    np.x = p.x * lengthInv;
    np.y = p.y * lengthInv;
    np.z = p.z * lengthInv;

    return np;
}

void rotateVector(PT &vect, PT axis, double ra) {
    axis = getUnitVectorOf(axis);
    // Rodrigues Formulae
    vect = vect*cos(ra) + (axis * (1-cos(ra)) * dotProduct(axis, vect)) + (axis*vect)*sin(ra);

}

void drawTraingle(double h, double r, double del) {
    glBegin(GL_TRIANGLES); {
        glVertex3f(0, 0, h);
        glVertex3f(r, -del/2, 0);
        glVertex3f(r, del/2, 0);

        glVertex3f(0, 0, 0);
        glVertex3f(r, -del/2, 0);
        glVertex3f(r, del/2, 0);
    } glEnd();
}

void drawRectangle(double h, double r, double del) {
    glBegin(GL_QUADS); {
        glVertex3f(r, -del/2, h);
        glVertex3f(r, -del/2, 0);
        glVertex3f(r, del/2, 0);
        glVertex3f(r, del/2, h);
    } glEnd();
}

void drawCone() {
    double radius = ballRadius * 0.2;
    double height = ballRadius * 0.9;
    double sectors = 100;
    double del = (2 * M_PI * radius) / sectors;
    double theta = (del*1.0 / radius) * 180/M_PI; // s = r * theta

    for (int i=0; i<sectors; i++) {
        glPushMatrix();

        glRotatef(theta * i, 0, 0, 1);
        drawTraingle(height, radius, del);

        glPopMatrix();
    }
}

void drawCylinder() {
    double radius = ballRadius * 0.1;
    double height = ballRadius * 0.9;

    double sectors = 100;
    double del = (2 * M_PI * radius) / sectors;
    double theta = (del*1.0 / radius) * 180/M_PI; // s = r * theta

    for (int i=0; i<sectors; i++) {
        glPushMatrix();

        glRotatef(theta * i, 0, 0, 1);
        drawRectangle(height, radius, del);

        glPopMatrix();
    }
}

void drawArrow() {
    double cylinderHeight = ballRadius * 0.5;

    glPushMatrix();

    glTranslatef(ballCenter.x, ballCenter.y, ballCenter.z);
    glColor3f(0, 0, 1);
    glRotatef(arrowAngle, 0, 0, 1);

    glTranslatef(0, ballRadius * 0.9, 0);
    glRotatef(-90, 1, 0, 0); // pointing to y-axis
    drawCylinder();
    glTranslatef(0, 0, cylinderHeight);
    drawCone();

    glPopMatrix();
}

void updateBallVelocityVector() {
    // theta is negative but the vector head is in 1st quadrant, so (x, y) = (+, +)
    // as sin(-theta) = -sin(theta), mult by - is required to get the positive / correct value

    ballVelocity.x = -sin(arrowAngle * M_PI /180);
    ballVelocity.y = cos(arrowAngle * M_PI /180);
    ballVelocity.z = 0.0f;

    //-90                    -> When arrowAngle is around multiple of 90
    //16.1232, 3e-17, 1e-06  -> velocity coordinates values can glitch and jump suddenly probably
    //                          because of some issue in implementation of sin()
    // Hence normalization is done below
    ballVelocity = getUnitVectorOf(ballVelocity);
}

void updateBallDirAndAngle() {
    if (manualMode == 1) {
        double dx1 = abs(wallLength/2 - ballCenter.x) - ballRadius;
        double dx2 = abs(-wallLength/2 - ballCenter.x) - ballRadius;
        
        double dy1 = abs(wallLength/2 - ballCenter.y) - ballRadius;
        double dy2 = abs(-wallLength/2 - ballCenter.y) - ballRadius;
        if (dx1 <= 0.05 or dx2 <= 0.05) {
            ballVelocity.x = -ballVelocity.x;
            if (ballVelocity.y < 0)  arrowAngle = 180 - atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
            else arrowAngle =-atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
        }
        if (dy1 <= 0.05 or dy2 <= 0.05) {
            ballVelocity.y = -ballVelocity.y;
            if (ballVelocity.y < 0)  arrowAngle = 180 - atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
            else arrowAngle = -atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
        }
    }
}

bool isCollision() {
    if (manualMode == 0) {
        double dx1 = abs(wallLength/2 - ballCenter.x) - ballRadius;
        double dx2 = abs(-wallLength/2 - ballCenter.x) - ballRadius;
        
        double dy1 = abs(wallLength/2 - ballCenter.y) - ballRadius;
        double dy2 = abs(-wallLength/2 - ballCenter.y) - ballRadius;
        if (dx1 <= 0.05 or dx2 <= 0.05 or dy1 <= 0.05 or dy2 <= 0.05) return true;
        return false;
    }
    return false;
}


void createBall(double radius) {
    double x, y, z, xy;                              // vertex position
    double nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    double s, t;                                     // vertex texCoord

    double sectorStep = 2 * M_PI * 1.0 / segmentCount;
    double stackStep = M_PI / segmentCount;
    double sectorAngle, stackAngle;

    for(int i = 0; i <= segmentCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for(int j = 0; j <= segmentCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            points[i][j].x = x;
            points[i][j].y = y;
            points[i][j].z = z;

        }
    }
}

void drawBall() {

    glPushMatrix();

        glTranslatef(ballCenter.x, ballCenter.y, ballCenter.z);
    
        for(int i=0; i < segmentCount; i++) {
            for(int j=0; j < segmentCount; j++) {
                ballColor = 1 - ballColor;
                if (ballColor) glColor3f(1.0, 0.0, 0.0);
                if (!ballColor) glColor3f(0.0, 1.0, 0.0);

                glBegin(GL_QUADS);
                {
                    glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                }
                glEnd();
                
            }

        }

    glPopMatrix();


}

void rotateBall(double dir) {
    PT k    = PT(0, 0, 1);
    PT axis = k*ballVelocity;

    for (int i=0; i <= segmentCount; i++) {
        for (int j=0; j <= segmentCount; j++) {
            rotateVector(points[i][j], axis, dir * perClickroll);
        }
    }

}

void translateBall(double dir, PT velocity = ballVelocity) {
    ballCenter = ballCenter + ballVelocity * (dir * ballRadius * perClickroll);
}


void rollBall(double dir, PT velocity = ballVelocity) {
    rotateBall(dir);
    translateBall(dir);
}


void drawWalls() {

    //walls[number of walls][4 points to define per wall]
    glPushMatrix();
    glTranslatef(0, 0, -ballRadius);
    glColor3f(1.0, 0.0, 0.0);

    PT wallPoints[4] = { 
        PT(wallLength/2, wallLength/2, 0), PT(wallLength/2, wallLength/2, wallHeight), PT(-wallLength/2, wallLength/2, wallHeight), PT(-wallLength/2, wallLength/2, 0)             
                    };

    for (int i=0; i<4; i++) {
        glRotatef(90, 0, 0, 1);
        glBegin(GL_QUADS); {
            for (int j=0; j<4; j++)
                glVertex3f(wallPoints[j].x, wallPoints[j].y, wallPoints[j].z);
        } glEnd();
    }

    glPopMatrix();
}



void display(){
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background ballColor to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     
    glLoadIdentity();

    gluLookAt(pos.x, pos.y, pos.z,	pos.x + lookat.x, pos.y + lookat.y, pos.z + lookat.z, up.x, up.y, up.z);
    
    drawAxes();
    drawBall();
    drawArrow();
    drawWalls();
    drawCheckerboard();
    
    glutSwapBuffers();
}
void Timer(int);
void rollBallTimer(int);
void calculateCollisionTime() {
    double xVelocity = ballVelocity.x * ballRadius * perClickroll;
    double yVelocity = ballVelocity.y * ballRadius * perClickroll;

    double dx1 = wallLength/2 - ballCenter.x - ballRadius;
    double dx2 = -wallLength/2 - ballCenter.x - ballRadius;

    double dy1 = wallLength/2 - ballCenter.y - ballRadius;
    double dy2 = -wallLength/2 - ballCenter.y - ballRadius;

    double tx1 = dx1 / xVelocity;
    double tx2 = dx2 / xVelocity;
    double ty1 = dy1 / yVelocity;
    double ty2 = dy2 / yVelocity;
    
    timeToCollide = GL_MAX;
    if (tx1 > 0) timeToCollide = min(timeToCollide, tx1);
    if (tx2 > 0) timeToCollide = min(timeToCollide, tx2);
    if (ty1 > 0) timeToCollide = min(timeToCollide, ty1);
    if (ty2 > 0) timeToCollide = min(timeToCollide, ty2);
    
    if (timeToCollide == tx1 or timeToCollide == tx2) collisionDirection = X_COLLISION;
    else collisionDirection = Y_COLLISION;
    
    glutTimerFunc(timeToCollide * quanta, Timer, 0);
}

void ballSimulation() {
    if (manualMode == 0) {
        if (collisionDirection == X_COLLISION) {
            ballVelocity.x = -ballVelocity.x;

            if (ballVelocity.y < 0)  arrowAngle = 180 - atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
            else arrowAngle =-atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
        }
        else if (collisionDirection == Y_COLLISION) {
            ballVelocity.y = -ballVelocity.y;

            if (ballVelocity.y < 0)  arrowAngle = 180 - atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
            else arrowAngle = -atan(ballVelocity.x / ballVelocity.y) * 180/M_PI;
        }
        // Scheduling the next collision
    }
}

void Timer(int value){
    if (manualMode == 0) {
        if (isCollision()) ballSimulation();
        else calculateCollisionTime();
    }
}

void rollBallTimer(int dir) {
    if (manualMode == 0) {
        rollBall(dir);
        glutTimerFunc(quanta, rollBallTimer, 1);
    }
}

void idle(){
    glutPostRedisplay();
}

void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background ballColor to black and opaque
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40, 1, 1, 500.0);

    segmentCount = 10;
    sectorStep = 2 * M_PI / segmentCount;
    stackStep = M_PI / segmentCount;

    ballRadius = 0.25f;

    perClickChange = 0.1f;
    arrowAngle = 0.0f;  // In degrees
    perClickArrowAngleChange = 10.0f; // In degrees

    ballCenter.x = 0.0f;
    ballCenter.y = 0.0f;
    ballCenter.z = 0.0f;

    // Box length
    wallLength = 6;
    wallHeight = 1.0f;
    timeToCollide = 0.0f;

    updateBallVelocityVector();
    createBall(ballRadius);

    // Simulation time quanta
    quanta = 44; // in ms
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
    // Camera positioning
    for (int i=0; i<30; i++) pos = pos + (up * SCALE_FACTOR);
    for (int i=0; i<42; i++) {
        rotateVector(up, rightside, -rotation_angle);
        rotateVector(lookat, rightside, -rotation_angle);
    }


    glutTimerFunc(0, Timer, 0);
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
        // Rotate / look left
        case '1':
            rotateVector(rightside, up, rotation_angle);
            rotateVector(lookat, up, rotation_angle);
            break;

        // Rotate / look right
        case '2':
            rotateVector(rightside, up, -rotation_angle);
            rotateVector(lookat, up, -rotation_angle);
            break;

        // Look up rotation
        case '3':
            rotateVector(up, rightside, rotation_angle);
            rotateVector(lookat, rightside, rotation_angle);
            break;

        // Look down rotation
        case '4':
            rotateVector(up, rightside, -rotation_angle);
            rotateVector(lookat, rightside, -rotation_angle);
            break;

        // tilt counterclockwise
        case '5':
            cout << up << rightside << rotation_angle << endl;
            rotateVector(up, lookat, rotation_angle);
            rotateVector(rightside, lookat, rotation_angle);
            break;
        
        // tilt clockwise
        case '6':
            rotateVector(up, lookat, -rotation_angle);
            rotateVector(rightside, lookat, -rotation_angle);
            break;


        case 'i':
            rollBall(1, ballVelocity);
            // Checking for collision
            updateBallDirAndAngle();

            break;

        case 'k':
            rollBall(-1, ballVelocity);
            // Checking for collision
            updateBallDirAndAngle();
            
            break;

        case 'j':
            arrowAngle += perClickArrowAngleChange;
            updateBallVelocityVector();
            break;

        case 'l':
            arrowAngle -= perClickArrowAngleChange;
            updateBallVelocityVector();
            break;

        case ' ':
            manualMode = 1 - manualMode;
            calculateCollisionTime();
            glutTimerFunc(0, rollBallTimer, 1);
            break;

        default:
            cout << "Invalid Key Pressed" << endl;
            break;
    }
}
