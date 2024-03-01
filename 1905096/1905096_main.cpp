#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <GL/glut.h>
#include <windows.h>
#endif

#include "bitmap_image.hpp"
#include "1905096_classes.h"

void keyboardListener(unsigned char, int, int);
void specialKeyListener(int, int, int);

const double rotation_angle = M_PI * 2 / 180;
const double rotate90 = M_PI / 2;
#define SCALE_FACTOR 4

// Variable declaration section
int levelOfRecursion, imageDimension;
vector<Object *> objects;
vector<PointLight *> pointLights;
vector<SpotLight *> spotLights;
bitmap_image image;

// Camera variables
double windowHeight = 750, windowWidth = 750;
double fovY = 40;

double perClickChange;
double cubeRotationAngle;
//  ------------------------------------ x --------------------------------------

PT pos, up, rightside, lookat;

using namespace std;

void drawAxes()
{
    glBegin(GL_LINES);
    {
        glColor3f(1.0, 0.0, 0.0); // X axis
        glVertex3f(20, 0, 0);
        glVertex3f(-20, 0, 0);

        glColor3f(0.0, 1.0, 0.0); // Y axis
        glVertex3f(0, -20, 0);
        glVertex3f(0, 20, 0);

        glColor3f(0.0, 0.0, 1.0); // Z axis
        glVertex3f(0, 0, 20);
        glVertex3f(0, 0, -20);
    }
    glEnd();
}

void checkColorThreshold(Color &color)
{
    // Clip the color values so that they are in [0, 1] range.
    if (color.r > 1)
        color.r = 1;
    else if (color.r < 0)
        color.r = 0;
    if (color.g > 1)
        color.g = 1;
    else if (color.g < 0)
        color.g = 0;
    if (color.b > 1)
        color.b = 1;
    else if (color.b < 0)
        color.b = 0;
}
int cnt = 1;

void capture()
{
    image.set_all_channels(0, 0, 0);

    double planeDistance = (windowHeight / 2) / tan((M_PI * fovY) / (180 * 2.0));
    PT topLeft = pos + (lookat * planeDistance) - (rightside * (windowWidth / 2.0)) + (up * (windowHeight / 2.0));

    double du = (double)windowWidth / imageDimension;
    double dv = (double)windowHeight / imageDimension;
    // Choose the middle of the topLeft pixel
    topLeft = topLeft + (rightside * (du / 2.0)) - (up * (dv / 2.0));

    for (int i = 0; i < imageDimension; i++)
    {
        for (int j = 0; j < imageDimension; j++)
        {
            // Calculate the current pixel
            PT currentPixel = topLeft + (rightside * du * i) - (up * dv * j);

            // Ray from eye to object Ray(start, dir)
            Ray ray(pos, currentPixel - pos);
            Color color(0, 0, 0); // No color ray at the beginning

            Object *nearestObject = nullptr;
            double t = -1, t_min = INT_MAX;

            for (Object *obj : objects)
            {
                t = obj->getIntersection(ray, color, 0); // No recursive ray tracing (level = 0)

                if (t > 0)
                {
                    if ((nearestObject == nullptr) or (t < t_min))
                    {
                        t_min = t;
                        nearestObject = obj;
                    }
                }
            }

            if (nearestObject != nullptr) // Found a nearest object
            {
                color = Color(0, 0, 0);
                t_min = nearestObject->getIntersection(ray, color, 1); // Recursive ray tracing

                checkColorThreshold(color);
                double red = color.r * 255;
                double green = color.g * 255;
                double blue = color.b * 255;

                image.set_pixel(i, j, red, green, blue);
            }
        }
    }

    image.save_image("Output_1" + to_string(cnt) + ".bmp");
    cnt++;
}

void display()
{

    glClearColor(0.0, 0.0, 0.0, 1.0);                    // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(pos.x, pos.y, pos.z, pos.x + lookat.x, pos.y + lookat.y, pos.z + lookat.z, up.x, up.y, up.z);

    drawAxes();

    // Draw the objects
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
    // Draw the Point Lights
    for (int i = 0; i < pointLights.size(); i++)
    {
        pointLights[i]->draw();
    }
    // Draw the Spot Lights
    for (int i = 0; i < spotLights.size(); i++)
    {
        spotLights[i]->draw();
    }

    // glFlush();
    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay();
}

void readData()
{
    ifstream file("scene.txt");
    if (!file.is_open())
    {
        cout << "File not found" << endl;
        return;
    }

    string type;
    double x, y, z, radius;
    Color color;
    Object *obj;
    Coef coEfficients;
    int shine;

    // Floor
    obj = new Floor(1000, 20);
    obj->setColor(Color(0.941176, 1, 0.8));
    obj->setCoEfficients(Coef(0.4, 0.2, 0.2, 0.2));
    objects.push_back(obj);

    file >> levelOfRecursion;
    file >> imageDimension;

    int objCount;
    file >> objCount;

    for (int i = 0; i < objCount; i++)
    {
        file >> type;

        if (type.compare("sphere") == 0)
        {
            file >> x >> y >> z;
            file >> radius;
            file >> color.r >> color.g >> color.b;
            file >> coEfficients.amb >> coEfficients.diff >> coEfficients.spec >> coEfficients.refl;
            file >> shine;

            obj = new Sphere(PT(x, y, z), radius);
            obj->setColor(Color(color.r, color.g, color.b));
            obj->setCoEfficients(Coef(coEfficients.amb, coEfficients.diff, coEfficients.spec, coEfficients.refl));
            obj->setShine(shine);

            objects.push_back(obj);
        }
        else if (type.compare("triangle") == 0)
        {
            vector<PT> points;
            for (int j = 0; j < 3; j++)
            {
                file >> x >> y >> z;
                points.push_back(PT(x, y, z));
            }

            obj = new Triangle(points[0], points[1], points[2]);
            file >> color.r >> color.g >> color.b;
            file >> coEfficients.amb >> coEfficients.diff >> coEfficients.spec >> coEfficients.refl;
            file >> shine;

            obj->setColor(Color(color.r, color.g, color.b));
            obj->setCoEfficients(Coef(coEfficients.amb, coEfficients.diff, coEfficients.spec, coEfficients.refl));
            obj->setShine(shine);

            objects.push_back(obj);
        }
        else if (type.compare("general") == 0)
        {
            double a, b, c, d, e, f, g, h, i, j;
            file >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
            double length, width, height;
            file >> x >> y >> z >> length >> width >> height;
            file >> color.r >> color.g >> color.b;
            file >> coEfficients.amb >> coEfficients.diff >> coEfficients.spec >> coEfficients.refl;
            file >> shine;

            obj = new General(a, b, c, d, e, f, g, h, i, j, PT(x, y, z), length, width, height);
            obj->setColor(Color(color.r, color.g, color.b));
            obj->setCoEfficients(Coef(coEfficients.amb, coEfficients.diff, coEfficients.spec, coEfficients.refl));
            obj->setShine(shine);

            objects.push_back(obj);
        }
    }

    int pointLightCount;
    file >> pointLightCount;

    for (int i = 0; i < pointLightCount; i++)
    {
        file >> x >> y >> z;
        file >> color.r >> color.g >> color.b;

        PointLight *pl = new PointLight(PT(x, y, z), color);

        pointLights.push_back(pl);
    }

    int spotLightCount;
    file >> spotLightCount;

    double directionX, directionY, directionZ, cutOffAngle;

    for (int i = 0; i < spotLightCount; i++)
    {
        file >> x >> y >> z;
        file >> color.r >> color.g >> color.b;
        file >> directionX >> directionY >> directionZ;
        file >> cutOffAngle;

        SpotLight *sl = new SpotLight(PT(x, y, z), PT(directionX, directionY, directionZ), color, cutOffAngle);

        spotLights.push_back(sl);
    }

    file.close();
}

void setCamPos()
{
    // position of camera
    pos = PT(200, 0, 10);

    // up, right and look direction
    up = PT(0, 0, 1);
    rightside = PT(-1 / sqrt(2), 1 / sqrt(2), 0);
    lookat = PT(-1 / sqrt(2), -1 / sqrt(2), 0);
}

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fovY, 1, 1, 1000.0);

    readData();
    setCamPos();

    perClickChange = 0.1;
    image = bitmap_image(imageDimension, imageDimension);
}

void clearMem() {
    for (int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }
    for (int i = 0; i < pointLights.size(); i++)
    {
        delete pointLights[i];
    }
    for (int i = 0; i < spotLights.size(); i++)
    {
        delete spotLights[i];
    }

    objects.clear();
    pointLights.clear();
    spotLights.clear();
    image.clear();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight); // Set the window's initial width & height
    glutInitWindowPosition(750, 0);                // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Ray Tracing");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutIdleFunc(idle);

    init();

    glutMainLoop();

    clearMem();
    return 0;
}

void specialKeyListener(int key, int x, int y)
{
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

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        exit(0);
        break;
    case '0':
        capture();
        cout << "Image captured" << endl;
        break;
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
        rotateVector(up, lookat, rotation_angle);
        rotateVector(rightside, lookat, rotation_angle);
        break;

    // titl clockwise
    case '6':
        rotateVector(up, lookat, -rotation_angle);
        rotateVector(rightside, lookat, -rotation_angle);
        break;

    default:
        cout << "Invalid Key Pressed" << endl;
        break;
    }
}