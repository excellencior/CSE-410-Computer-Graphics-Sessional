#include<bits/stdc++.h>
#include"bitmap_image.hpp"
using namespace std;

typedef vector<vector<double>> vvd;

class PT {
public:
    double x, y, z, w;

    PT() {
        // Required for non-parameterized object creation
    }

    PT(double x, double y, double z, double w = 1.0) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    // Copy constructor
    PT(const PT &p) {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
        this->w = p.w;
    }

    PT& operator = (const PT &ob2) {
        this->x = ob2.x;
        this->y = ob2.y;
        this->z = ob2.z;
        this->w = ob2.w;
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

// vector * matrix
PT operator * (const PT &ob, const vvd &mat) {
    vector<double> point({ob.x, ob.y, ob.z, ob.w}), res(4);
    for (int i=0; i<4; i++) {
        double sum = 0.0;
        for (int j=0; j<4; j++) {
            sum += point[j] * mat[i][j];
        }
        res[i] = sum;
    }
    
    return PT(res[0], res[1], res[2], res[3]);
}

// vector / const
PT operator / (const PT &ob, const double num) {
    return PT(ob.x*1.0 / num, ob.y*1.0 / num, ob.z*1.0 / num);
}

std::ostream& operator<<(std::ostream& os, const PT& ob) {
    os << "x = " << ob.x << ", y = " << ob.y << ", z = " << ob.z << ", w = " << ob.w << std::endl;
    return os;
}

double dotProduct(PT v1, PT v2) {
    double res = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    return res;
}

class LookAtCamera {
    private:
        PT eye, lookat, up;
    public:
        LookAtCamera() {
            // Required for non-parameterized object creation
        }
        LookAtCamera(PT eye, PT lookat, PT up) {
            this->eye = eye;
            this->lookat = lookat;
            this->up = up;
        }

        void setLookAt(PT eye, PT lookat, PT up) {
            this->eye = eye;
            this->lookat = lookat;
            this->up = up;
        }

        // Getters
        PT getEye() {
            return this->eye;
        }
        PT getLookat() {
            return this->lookat;
        }
        PT getUp() {
            return this->up;
        }
};

class PerspectiveCamera {
    private:
        double fovY, aspectRatio, near, far;
    public:
        PerspectiveCamera() {
            // Required for non-parameterized object creation
        }
        PerspectiveCamera(double fovY, double aspectRatio, double near, double far) {
            this->fovY = fovY;
            this->aspectRatio = aspectRatio;
            this->near = near;
            this->far = far;
        }

        void setPerspectiveCamera(double fovY, double aspectRatio, double near, double far) {
            this->fovY = fovY;
            this->aspectRatio = aspectRatio;
            this->near = near;
            this->far = far;
        }

        // Getters
        double getFovY() {
            return this->fovY;
        }
        double getAspectRatio() {
            return this->aspectRatio;
        }
        double getNear() {
            return this->near;
        }
        double getFar() {
            return this->far;
        }
};

class Utility {
    private:
        unsigned long int g_seed = time(0);
    public:
        // Default Constructor
        Utility() {}
        double modulusOf(PT p) {
            double sum = (p.x * p.x) + (p.y * p.y) + (p.z * p.z);
            return sqrt(sum);
        }

        PT getUnitVectorOf(PT p) {
            double lengthInv = 1.0f / modulusOf(p);

            PT np;
            np.x = p.x * lengthInv;
            np.y = p.y * lengthInv;
            np.z = p.z * lengthInv;

            return np;
        }

        void rotateVector(PT &vect, PT axis, double ra) {
            axis = getUnitVectorOf(axis);
            ra = ra * M_PI / 180.0; // Convert to radians
            // Rodrigues Formulae
            vect = vect*cos(ra) + (axis * (1-cos(ra)) * dotProduct(axis, vect)) + (axis*vect)*sin(ra);
        }

        vvd multiplyMatrices(vvd mat1, vvd mat2) {
            vvd res = vvd(4, vector<double>(4, 0.0));
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    res[i][j] = 0.0;
                    for(int k=0; k<4; k++) {
                        res[i][j] += mat1[i][k] * mat2[k][j];
                    }
                }
            }
            return res;
        }

        inline int random()
        {
            g_seed = (214013 * g_seed + 2531011);
            return ((g_seed >> 16) & 0x7FFF) % 256;
        }

        void print(vvd matrix) {
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    cout << fixed << setprecision(7) << matrix[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
};

// A class that represents a transformation matrix which has 3 2d-arrays denoting rotation, translation and scaling
class Transformation {
    private:
        // Utility object
        Utility util;

        vvd matrix;
        vector<PT> principleAxis;

        void initPrincipalAxis() {
            // Initialize the principle axis
            this->principleAxis.push_back(PT(1.0, 0.0, 0.0));
            this->principleAxis.push_back(PT(0.0, 1.0, 0.0));
            this->principleAxis.push_back(PT(0.0, 0.0, 1.0));
        }

    public:
        Transformation() {
            this->matrix = vvd(4, vector<double>(4));
            double val = 0.0;
            // Required for non-parameterized object creation
            // Initialize the matrix with identity matrix
            for (int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    if (i == j)
                        val = 1.0;
                    else val = 0.0;

                    this->matrix[i][j] = val;
                }
            }
            initPrincipalAxis();
        }
        Transformation(vvd matrix) {
            for (int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    this->matrix[i][j] = matrix[i][j];
                }
            }
            initPrincipalAxis();
        }

        void setTransformationMatrix(vvd matrix) {
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    this->matrix[i][j] = matrix[i][j];
                }
            }
        }

        void translate(double tx, double ty, double tz) {
            vvd translationMatrix = vvd(4, vector<double>(4, 0.0));
            for (int i=0; i<4; i++) {
                translationMatrix[i][i] = 1.0;
            }

            translationMatrix[0][3] = tx;
            translationMatrix[1][3] = ty;
            translationMatrix[2][3] = tz;

            this->matrix = util.multiplyMatrices(this->matrix, translationMatrix);
        }

        void scale(double sx, double sy, double sz) {
            vvd scalingMatrix = vvd(4, vector<double>(4, 0.0));
            scalingMatrix[0][0] = sx;
            scalingMatrix[1][1] = sy;
            scalingMatrix[2][2] = sz;
            scalingMatrix[3][3] = 1.0;

            this->matrix = util.multiplyMatrices(this->matrix, scalingMatrix);
        }

        void rotate(double angle, double ax, double ay, double az) {
            PT rotationAxis = PT(ax, ay, az);
            // rotate i
            util.rotateVector(principleAxis[0], rotationAxis, angle);
            // rotate j
            util.rotateVector(principleAxis[1], rotationAxis, angle);
            // rotate k
            util.rotateVector(principleAxis[2], rotationAxis, angle);

            vvd rotationMatrix = vvd(4, vector<double>(4, 0.0));
            for(int i=0; i<3; i++) {
                rotationMatrix[0][i] = principleAxis[i].x;
                rotationMatrix[1][i] = principleAxis[i].y;
                rotationMatrix[2][i] = principleAxis[i].z;
            }
            rotationMatrix[3][3] = 1.0;

            this->matrix = util.multiplyMatrices(this->matrix, rotationMatrix);
        }

        // Getters
        vvd getModelTransformationMatrix() {
            return this->matrix;
        }

        Transformation& operator = (const Transformation &ob2) {
            copy(ob2.matrix.begin(), ob2.matrix.end(), this->matrix.begin());

            copy(ob2.principleAxis.begin(), ob2.principleAxis.end(), this->principleAxis.begin());

            return *this;
        }
};


class Triangle {
    private:
        vector<int> color; // RGB
        vector<PT> points;
    public:

        Triangle(PT p1, PT p2, PT p3, int r, int g, int b) {
            this->color = vector<int>(3);
            this->points = vector<PT>(3);

            this->points[0] = p1;
            this->points[1] = p2;
            this->points[2] = p3;
            this->color[0] = r;
            this->color[1] = g;
            this->color[2] = b;
        }

        // Getters
        vector<PT> getPoints() {
            return this->points;
        }
        vector<int> getColors() {
            return this->color;
        }

        friend ostream& operator<<(ostream& os, const Triangle& ob) {
            os << "Triangle: " << endl;
            for (int i=0; i<3; i++) {
                os << fixed << setprecision(7) << ob.points[i];
            }
            os << "Color: " << ob.color[0] << " " << ob.color[1] << " " << ob.color[2] << endl;
            return os;
        }
};

class ZBuffer {
    private:
        Utility util;
        fstream fin_config, fin_input, fout_buffer;
        int width, height;
        double ZMIN = -1.0, ZMAX = 1.0, x, y, z;
        vector<Triangle> triangles;
        double rightLimitX = 1.0, leftLimitX = -1.0, topLimitY = 1.0, bottomLimitY = -1.0; // Volume Box
        double **zBuffer;

        void initZBuffer(int height, int width) {
            zBuffer = new double*[width];
            for (int i=0; i<width; i++) {
                zBuffer[i] = new double[height];
            }
            for (int i=0; i<width; i++) {
                for (int j=0; j<height; j++) {
                    zBuffer[i][j] = ZMAX;
                }
            }
        }

        void clearZBuffer() {
            // clear the z-buffer
            for (int i=0; i<width; i++) {
                delete[] zBuffer[i];
            }
        }

        void writeZBufferToFile() {
            // Write the z-buffer to "z_buffer.txt"
            for (int i=0; i<height; i++) {
                for (int j=0; j<width; j++) {
                    if (zBuffer[j][i] < ZMAX)
                        fout_buffer << fixed << setprecision(6) << zBuffer[j][i] << "\t";
                }
                fout_buffer << endl;
            }
        }

        double intersect(double x1, double y1, double x2, double y2, double y) {
            double m = (y2 - y1) / (x2 - x1);
            double x = x1 + (y - y1) / m;
            return x;
        }

        void adjust(double &za, double &zb, double prev_xa, double prev_xb, double xa, double xb) {
            if (xa != prev_xa) {
                za = intersect(za, prev_xa, zb, prev_xb, xa);
            }
            if (xb != prev_xb) {
                zb = intersect(za, prev_xa, zb, prev_xb, xb);
            }
        }

    public:
        ZBuffer(string config_file, string input_file, string output_image, string output_zbuffer) {
            fin_config.open(config_file, ios::in);
            fin_input.open(input_file, ios::in);
            fout_buffer.open(output_zbuffer, ios::out);

            fin_config >> width >> height;
        }

        void printZBuffer() {
            // Read triangle objects from "stage3.txt"
            bool eof_file = false;
            while (true) {
                vector<PT> points;
                // Per 3 lines denote a triangle
                for (int i=0; i<3; i++) {
                    if (fin_input >> x >> y >> z)
                        points.push_back(PT(x, y, z));
                    else {
                        eof_file = true;
                        break;
                    }
                }
                if (eof_file) break;
                int red_channel = util.random();
                int green_channel = util.random();
                int blue_channel = util.random();
                
                triangles.push_back(Triangle(points[0], points[1], points[2], red_channel, green_channel, blue_channel));
            }
            // Checking if the read is correct
            // for (Triangle t: triangles) {
            //     cout << t << endl;
            // }

            // Image bounding box limits
            double dx = (double)(rightLimitX - leftLimitX) / width;
            double dy = (double)(topLimitY - bottomLimitY) / height;

            // Middle points of the pixels (screen's cell centers)
            double left_x = leftLimitX + dx / 2.0;
            double right_x = rightLimitX - dx / 2.0;
            double top_y = topLimitY - dy / 2.0;
            double bottom_y = bottomLimitY + dy / 2.0;

            // During scanning from top to bottom and left to right, check for the middle values of each cell

            // Initialize the z-buffer
            initZBuffer(height, width); // height == #rows and width == #columns

            // Create a bitmap image with the given width and height
            bitmap_image image(width, height);
            // Set the background color to black
            image.set_all_channels(0);

            for (Triangle triangle: triangles) {
                double minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
                // Find the min and max of x and y coordinates of the triangle's corners
                for (PT p: triangle.getPoints()) {
                    minx = min(minx, p.x);
                    maxx = max(maxx, p.x);
                    miny = min(miny, p.y);
                    maxy = max(maxy, p.y);
                }
                // cout << triangle << endl;

                // Clipping
                minx = max(minx, left_x);
                maxx = min(maxx, right_x);
                miny = max(miny, bottom_y);
                maxy = min(maxy, top_y);

                // Find the maxY scanline and minY scanline
                int maxYScanline = round((top_y - maxy) / dy);
                int minYScanline = round((top_y - miny) / dy);
                // Why divide by dy? World Space to Raster Space (0, 1, 2...)

                // Scan from top to bottom (along Y axis)
                for (int scanning_y = maxYScanline; scanning_y <= minYScanline; scanning_y++) {
                    double scanlineY = top_y - scanning_y * dy; // In world space
                    
                    // find the intersection points of the scanline with the triangle, where the scanline may intersect any two lines of the triangle
                    // For each edge of the triangle
                    vector<pair<double, double>> intersections;
                    PT p1, p2;
                    // For each edge of the triangle
                    for (int i = 0; i < 3; i++) {
                        p1 = triangle.getPoints()[i];
                        p2 = triangle.getPoints()[(i+1)%3];

                        // Check if the edge is horizontal
                        // Jodi horizontal hoy, division by zero error may occur, condition check if (1st condn)

                        // Intersection point 2 ta hobe at max except if the scanline intersects a top point of the triangle
                        // Jodi scanlineY duita point er line er majhe diye jay, tahole oder Y range er majhe thakbe
                        // z-cor ta ber korte hobe, triangle ta y/x axis or any other vector er respect e rotated thakte pare
                        if ((p1.y != p2.y) and (scanlineY >= min(p1.y, p2.y)) and (scanlineY <= max(p1.y, p2.y))) {
                            x = intersect(p1.x, p1.y, p2.x, p2.y, scanlineY);
                            z = intersect(p1.z, p1.y, p2.z, p2.y, scanlineY);

                            intersections.push_back({x, z});
                        }
                    }
                    if (intersections.size() == 0) continue; // due to precision loss (-.002 not 0)

                    // Sort the intersection points by x-coordinate
                    std::sort(intersections.begin(), intersections.end(), [](const auto& a, const auto& b) {
                        return a.first < b.first;
                    });

                    // Case: Hote pare je triangle er x-cor out of the volume box, tai jodi oita intersecting point hoye thake ba choto ba boro hoy then x-clipping ou korte hobe
                    double prev_xa = intersections[0].first, prev_xb = intersections[1].first;
                    double prev_za = intersections[0].second, prev_zb = intersections[1].second;

                    // Case: Jodi x-cor clip kora hoy, z-cor ta change hoyeche, tai z-cor update korte hobe
                    // Etar jonno previous x-cor ta lagbe, regression formulae ta use korte hole

                    double xa = intersections[0].first, xb = intersections[1].first, za = intersections[0].second, zb = intersections[1].second;
                    
                    if (xa < minx) xa = minx;
                    if (xa > maxx) xa = maxx;
                    if (xb < minx) xb = minx;
                    if (xb > maxx) xb = maxx;

                    // Case: Jodi x-cor clip kora hoy, z-cor ta change hoyeche, tai z-cor update korte hobe
                    adjust(za, zb, prev_xa, prev_xb, xa, xb);
                    // za = zb - (zb - za) * (prev_xb - xa) / (prev_xb - prev_xa);
                    // zb = zb - (zb - za) * (prev_xb - xb) / (prev_xb - prev_xa);

                    double minXScanline = round((xa - left_x) / dx);
                    double maxXScanline = round((xb - left_x) / dx);

                    // Scan from left to right (along X axis)
                    for (int scanning_x = minXScanline; scanning_x <= maxXScanline; scanning_x++) {
                        double scanlineX = left_x + scanning_x * dx; // In world space

                        // we know the (x, y) of the pixel, we need to find the z
                        z = intersect(za, xa, zb, xb, scanlineX);
                        // double z = zb - (zb - za) * (xb - scanlineX) / (xb - xa);
                        // Case: Jodi z-buffer er value boro hoy, tahole oita update korte hobe
                        // z also has to be greater than z_front_limit
                        if (z < zBuffer[scanning_x][scanning_y] and z >= ZMIN) {
                            zBuffer[scanning_x][scanning_y] = z;

                            // Case: Jodi z-buffer er value boro hoy, tahole oita update korte hobe
                            // Color update korte hobe
                            int red_channel = triangle.getColors()[0];
                            int green_channel = triangle.getColors()[1];
                            int blue_channel = triangle.getColors()[2];

                            // Image being the frame buffer
                            image.set_pixel(scanning_x, scanning_y, red_channel, green_channel, blue_channel);
                        }
                    }
                }
            }

            writeZBufferToFile();

            image.save_image("out.bmp"); // Save the image to "out.bmp"

            fin_config.close();
            fin_input.close();
            fout_buffer.close();

            clearZBuffer();
        }
};

LookAtCamera EyeLookUp;
PerspectiveCamera camera;
Transformation transformation; // Default construction is identity matrix
Utility util;
string cmd;

vvd getViewTransformationMatrix() {
    vvd eyeMatrix = vvd(4, vector<double>(4, 0.0));
    for (int i=0; i<4; i++) {
        eyeMatrix[i][i] = 1.0;
    }
    eyeMatrix[0][3] = -EyeLookUp.getEye().x;
    eyeMatrix[1][3] = -EyeLookUp.getEye().y;
    eyeMatrix[2][3] = -EyeLookUp.getEye().z;

    PT l = EyeLookUp.getLookat() - EyeLookUp.getEye();
    l = util.getUnitVectorOf(l);
    PT r = l * EyeLookUp.getUp();
    r = util.getUnitVectorOf(r);
    PT u = r * l;

    vvd rotationMatrix = vvd(4, vector<double>(4, 0.0));
    rotationMatrix[0] = {r.x, r.y, r.z, 0.0};
    rotationMatrix[1] = {u.x, u.y, u.z, 0.0};
    rotationMatrix[2] = {-l.x, -l.y, -l.z, 0.0};
    rotationMatrix[3][3] = 1.0;

    vvd viewMatrix = util.multiplyMatrices(rotationMatrix, eyeMatrix);

    return viewMatrix;
}

vvd getProjectionTransformationMatrix() {
    double fovY = camera.getFovY();
    double aspectRatio = camera.getAspectRatio();
    double near = camera.getNear();
    double far = camera.getFar();

    double fovX = fovY * aspectRatio;
    double t = near * tan(fovY * M_PI / 360.0);
    double r = near * tan(fovX * M_PI / 360.0);

    vvd projectionMatrix = vvd(4, vector<double>(4, 0.0));
    projectionMatrix[0][0] = near / r;
    projectionMatrix[1][1] = near / t;
    projectionMatrix[2][2] = -(far + near) / (far - near);
    projectionMatrix[2][3] = -(2.0 * far * near) / (far - near);
    projectionMatrix[3][2] = -1.0;

    return projectionMatrix;
}

int main() {
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
    tStack.push(transformation); // Identity Matrix

    while (fin >> cmd) {
        
        if (cmd == "triangle") {
            for (int i=0; i<3; i++) {
                fin >> x >> y >> z;

                PT p = PT(x, y, z);

                p =  p * transformation.getModelTransformationMatrix();
                fout_1 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;

                p = p * getViewTransformationMatrix();
                fout_2 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;

                p = p * getProjectionTransformationMatrix();
                p = p / p.w; // Homogenize
                fout_3 << fixed << setprecision(7) << p.x << " " << p.y << " " << p.z << endl;
            }
            fout_1 << endl;
            fout_2 << endl;
            fout_3 << endl;
        }
        else if (cmd == "scale") {
            fin >> x >> y >> z;
            transformation.scale(x, y, z);
        }
        else if (cmd == "translate") {
            fin >> x >> y >> z;
            transformation.translate(x, y, z);
        }
        else if (cmd == "rotate") {
            double angle;
            fin >> angle >> x >> y >> z;
            transformation.rotate(angle, x, y, z);
        }
        else if (cmd == "push") {
            tStack.push(transformation);
        }
        else if (cmd == "pop") {
            transformation = tStack.top();
            tStack.pop();
        }
        else if (cmd == "end") break;
    }

    fin.close();
    fout_1.close();
    fout_2.close();
    fout_3.close();

    // Z-Buffer
    ZBuffer zBuffer("config.txt", "stage3.txt", "out.bmp", "z_buffer.txt");
    zBuffer.printZBuffer();
    

    return 0;
}