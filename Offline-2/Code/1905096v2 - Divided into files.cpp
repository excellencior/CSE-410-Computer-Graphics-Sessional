#include <bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

typedef vector<vector<double>> vvd;

#include "utility.h"
#include "triangle.h"
#include "lookat_camera.h"
#include "perspective_camera.h"
#include "model_transformation.h"
#include "view_transformation.h"
#include "projection_transformation.h"
#include "z_buffer.h"

LookAtCamera EyeLookUp;
PerspectiveCamera camera;
Transformation transformation; // Default construction is identity matrix
string cmd;

int main()
{
    fstream fin;
    fin.open("scene.txt", ios::in);
    fstream fout_1("stage1.txt", ios::out);
    fstream fout_2("stage2.txt", ios::out);
    fstream fout_3("stage3.txt", ios::out);

    double x, y, z;
    fin >> x >> y >> z;
    PT eye = PT(x, y, z);

    fin >> x >> y >> z;
    PT look = PT(x, y, z);

    fin >> x >> y >> z;
    PT up = PT(x, y, z);

    EyeLookUp.setLookAt(eye, look, up);

    double fovY, aspectRatio, near, far;
    fin >> fovY >> aspectRatio >> near >> far;
    camera.setPerspectiveCamera(fovY, aspectRatio, near, far);

    std::stack<Transformation> tStack; // Transformation Stack
    tStack.push(transformation);       // Identity Matrix

    while (fin >> cmd)
    {

        if (cmd == "triangle")
        {
            for (int i = 0; i < 3; i++)
            {
                fin >> x >> y >> z;

                PT p = PT(x, y, z);

                p = p * transformation.getModelTransformationMatrix();
                fout_1 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;

                p = p * getViewTransformationMatrix(EyeLookUp);
                fout_2 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;

                p = p * getProjectionTransformationMatrix(camera);
                p = p / p.w; // Homogenize
                fout_3 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;
            }
            fout_1 << endl;
            fout_2 << endl;
            fout_3 << endl;
        }
        else if (cmd == "scale")
        {
            fin >> x >> y >> z;
            transformation.scale(x, y, z);
        }
        else if (cmd == "translate")
        {
            fin >> x >> y >> z;
            transformation.translate(x, y, z);
        }
        else if (cmd == "rotate")
        {
            double angle;
            fin >> angle >> x >> y >> z;
            transformation.rotate(angle, x, y, z);
        }
        else if (cmd == "push")
        {
            tStack.push(transformation);
        }
        else if (cmd == "pop")
        {
            transformation = tStack.top();
            tStack.pop();
        }
        else if (cmd == "end")
            break;
    }

    fin.close();
    fout_1.close();
    fout_2.close();
    fout_3.close();

    // Z-Buffer
    ZBuffer zBuffer("config.txt", "stage3.txt", "out.bmp", "z_buffer.txt");
    zBuffer.calculateZBuffer();

    return 0;
}