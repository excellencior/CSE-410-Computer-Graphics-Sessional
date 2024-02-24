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

const double rotation_angle = M_PI * 2 / 180;
const double rotate90 = M_PI / 2;
#define SCALE_FACTOR 0.4

// Variable declaration section
int sectorCount, stackCount;
double sectorStep, stackStep;
double sectorAngle, stackAngle;

double maximumTriangleCornerDistance;
double triangleCornerDistance;
double maximumSphereRadius;
double sphereRadius;

double perClickChange;
double cubeRotationAngle;
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


void rotateVector(PT &vect, PT& axis, double ra) {
    vect = vect*cos(ra) + (axis*vect)*sin(ra);
}


double _rand(){
    return (double)rand() / RAND_MAX;
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

void drawSphereSquareCut() {
    struct PT points[stackCount+1][stackCount+1];
	double x,y;

    // Transforming a cube into a square
	for(int i=0; i <= stackCount; i++){
		x = -sphereRadius + i * 1.0/stackCount*(2 * sphereRadius);

		for(int j=0; j <= sectorCount; j++){
		    y = -sphereRadius + j * 1.0/sectorCount*(2 * sphereRadius);

            points[i][j] = getUnitVectorOf(PT(x, y, sphereRadius));
            points[i][j] = points[i][j]*sphereRadius;
		}
	}

    for(int i=0; i < stackCount; i++)
	{
		for(int j=0; j < sectorCount; j++)
		{
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
}

void drawSphereSquareCut_oldversion(double radius)
{
	struct PT points[stackCount+1][sectorCount+1];
	int i,j;

	double xy, x, y, z;

	for(i=0; i <= stackCount; i++){
        stackAngle = M_PI/2 - (i * stackStep) / 3;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle); 
		
		for(j=0; j <= sectorCount; j++){
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

			points[i][j].x = x;
			points[i][j].y = y;
			points[i][j].z = z;

            points[i][j] = getUnitVectorOf(points[i][j]);
            points[i][j] = points[i][j]*radius;
		}
	}
	//draw quads using generated points
	for(i=0; i < stackCount; i++)
	{
		for(j=0; j < sectorCount; j++)
		{
            glBegin(GL_QUADS);
            {
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
		}
	}
}

void drawSpheres() {
    for (int i=0; i<4; i++) {
        glPushMatrix();

        glRotatef(90 * i, 0, 1, 0);
        glTranslatef(0, 0, triangleCornerDistance);
        glColor3f((i + 1) % 2, 0, i % 2);
        drawSphereSquareCut();

        glPopMatrix();
    }

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glColor3f(0.0, 1.0, 0.0);

    for (int i=0; i<2; i++) {
        glPushMatrix();

        glRotatef(180 * i, 1, 0, 0);
        glTranslatef(0, 0, triangleCornerDistance);
        drawSphereSquareCut();

        glPopMatrix();
    }
    glPopMatrix();
}



void triangle() {
    glBegin(GL_TRIANGLES); {
        glVertex3f(0, 0, 1);
        glVertex3f(1, 0, 0);
        glVertex3f(0, 1, 0);
    } glEnd();
    
}

// 3-points of the base octahedron
void octahedron() {
    double originShiftDistance = (maximumTriangleCornerDistance - triangleCornerDistance) / 3.0f;

    for (int i=0; i<4; i++) {
        glPushMatrix();

        glColor3f(i % 2, (i + 1) % 2, 1.0);
        // Rotate a triangle wrt z axis
        glRotatef(90 * i, 0, 0, 1);
        glTranslatef(originShiftDistance, originShiftDistance, originShiftDistance);
        // Translate the triangle corner coordinates
        glScaled(triangleCornerDistance, triangleCornerDistance, triangleCornerDistance);
        triangle();

        glPopMatrix();
    }
    

    for (int i=0; i< 4; i++) {
        glPushMatrix();

        glColor3f((i + 1) % 2, i % 2, 1.0);
        // Rotate a triangle wrt z axis
        glRotatef(90 * i, 0, 0, 1);
        glRotatef(-90, 1, 0, 0);
        // Translate the triangle corner coordinates
        glTranslatef(originShiftDistance, originShiftDistance, originShiftDistance);
        glScaled(triangleCornerDistance, triangleCornerDistance, triangleCornerDistance);
        triangle();

        glPopMatrix();
    }
}

void cylinder(double height, double radius) {
    if (radius <= 0.0000001f) return;

    double cutAngle = 70.5287794f;

    for (double a = -(double)cutAngle/2; a <= (double)cutAngle/2; a+=0.1) {
        glPushMatrix();

        glRotatef(a, 0, 0, 1);

        glBegin(GL_QUADS); {
            glVertex3f(0, radius, -(double)height / 2);
            glVertex3f(0.01, radius, -(double)height / 2);
            glVertex3f(0.01, radius, (double)height / 2);
            glVertex3f(0, radius, (double)height / 2);
        } glEnd();

        glPopMatrix();
    }

}

void drawCylinder() {

    for (int i=0; i<4; i++) {
        glPushMatrix();
        
        glColor3f(1, 1, 0.0);
        glRotatef(90 * i, 0, 0, 1);
        glRotatef(-45, 1, 0, 0);
        
        glTranslatef(0, triangleCornerDistance / sqrt(2.0), 0);
        cylinder(sqrt(2.0) * triangleCornerDistance, sphereRadius);

        glPopMatrix();
    }


    for (int i=0; i<4; i++) {
        glPushMatrix();

        glRotatef(90 * i, 0, 0, 1);
        glRotatef(45, 1, 0, 0);
        glColor3f(1, 1, 0.0);
        
        glTranslatef(0, triangleCornerDistance / sqrt(2.0), 0);
        cylinder(sqrt(2.0) * triangleCornerDistance, sphereRadius);

        glPopMatrix();
    }


    for (int i=0; i<4; i++) {
        glPushMatrix();

        glRotatef(45 + 90*i, 0, 0, 1);
        glTranslatef(0, triangleCornerDistance/sqrt(2.0), 0);
        glRotatef(90, 0, 1, 0);
        glColor3f(1, 1, 0.0);
        cylinder(sqrt(2.0) * triangleCornerDistance, sphereRadius);

        glPopMatrix();
    }

}

void drawTheMagicCube() {
    glPushMatrix();

    glRotatef(2* cubeRotationAngle * 180/ M_PI, 0, 0, 1);
    octahedron();
    glColor3f(1.0, 1.0, 0.0);
    drawSpheres();
    drawCylinder();

    glPopMatrix();
}

void display(){
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     
    glLoadIdentity();

    gluLookAt(pos.x, pos.y, pos.z,	pos.x + lookat.x, pos.y + lookat.y, pos.z + lookat.z, up.x, up.y, up.z);
    // gluLookAt(pos.x, pos.y, pos.z,	lookat.x, lookat.y, lookat.z, up.x, up.y, up.z);
    drawAxes();

    drawTheMagicCube();

    // glFlush();
    glutSwapBuffers();
}

void idle(){
    glutPostRedisplay();
}

void init(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40, 1, 1, 1000.0);
    
    // Defiing constants
    sectorCount = 100;
    stackCount = 100;
    sectorStep = 2 * M_PI / sectorCount;
    stackStep = M_PI / stackCount;

    maximumTriangleCornerDistance = 1.0;
    triangleCornerDistance = 1.0;
    maximumSphereRadius = maximumTriangleCornerDistance / sqrt(3.0);
    sphereRadius = 0.0f;

    perClickChange = 0.1;
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
        case 'a':
            cubeRotationAngle -= rotation_angle;
            break;

        case 'd':
            cubeRotationAngle += rotation_angle;
            break;

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
        
        // titl clockwise
        case '6':
            rotateVector(up, lookat, -rotation_angle);
            rotateVector(rightside, lookat, -rotation_angle);
            break;

        case ',':
            if (triangleCornerDistance > 0.0001) {
                triangleCornerDistance -= perClickChange;
                sphereRadius += (maximumSphereRadius * perClickChange);
            }
            break;

        case '.':
            if (triangleCornerDistance < maximumTriangleCornerDistance) {
                triangleCornerDistance += perClickChange;
                sphereRadius -= (maximumSphereRadius * perClickChange);
            }
            break;

        default:
            cout << "Invalid Key Pressed" << endl;
            break;
    }
}