#ifndef CLASSES_H
#define CLASSES_H

// Forward declaration
class Object;
class PointLight;
class SpotLight;

extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;
extern int levelOfRecursion;

struct PT
{
    double x, y, z;

    PT()
    {
        // Required for non-parameterized object creation
    }

    PT(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // Copy constructor
    PT(const PT &p)
    {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
    }

    PT &operator=(const PT &ob2)
    {
        this->x = ob2.x;
        this->y = ob2.y;
        this->z = ob2.z;
        return *this;
    }
};

// Operator Overloading functions
PT operator+(const PT &ob1, const PT &ob2)
{
    return PT(ob1.x + ob2.x, ob1.y + ob2.y, ob1.z + ob2.z);
}

PT operator-(const PT &ob1, const PT &ob2)
{
    return PT(ob1.x - ob2.x, ob1.y - ob2.y, ob1.z - ob2.z);
}

// Cross product
PT operator*(const PT &ob1, const PT &ob2)
{
    return PT((ob1.y * ob2.z) - (ob2.y * ob1.z), (ob2.x * ob1.z) - (ob1.x * ob2.z), (ob1.x * ob2.y) - (ob2.x * ob1.y));
}

// vector * const
PT operator*(const PT &ob, const double num)
{
    return PT(ob.x * num, ob.y * num, ob.z * num);
}

// vector / const
// vector * const
PT operator/(const PT &ob, const double num)
{
    return PT(ob.x * 1.0 / num, ob.y * 1.0 / num, ob.z * 1.0 / num);
}

std::ostream &operator<<(std::ostream &os, const PT &ob)
{
    os << "x = " << ob.x << ", y = " << ob.y << ", z = " << ob.z << std::endl;
    return os;
}

double dotProduct(PT v1, PT v2)
{
    double res = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    return res;
}

double modulusOf(struct PT p)
{
    double sum = (p.x * p.x) + (p.y * p.y) + (p.z * p.z);
    return sqrt(sum);
}

PT getUnitVectorOf(struct PT p)
{
    double lengthInv = 1.0f / modulusOf(p);

    struct PT np;
    np.x = p.x * lengthInv;
    np.y = p.y * lengthInv;
    np.z = p.z * lengthInv;

    return np;
}

void rotateVector(PT &vect, PT axis, double ra)
{
    axis = getUnitVectorOf(axis);
    // Rodrigues Formulae
    vect = vect * cos(ra) + (axis * (1 - cos(ra)) * dotProduct(axis, vect)) + (axis * vect) * sin(ra);
}

pair<double, double> getQuadraticSolution(double a, double b, double c) {
    double dis = (b * b) - (4 * a * c);
    if (dis < 0) return {-1, -1};

    double t1 = (-b + sqrt(dis)) / (2 * a);
    double t2 = (-b - sqrt(dis)) / (2 * a);
    return {t1, t2};
}

class Color
{
public:
    double r, g, b;

    Color()
    {
        r = 0;
        g = 0;
        b = 0;
    }

    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    Color(const Color &color)
    {
        this->r = color.r;
        this->g = color.g;
        this->b = color.b;
    }

    Color operator*(const double coef)
    {
        return Color(r * coef, g * coef, b * coef);
    }

    Color operator*(const Color &c)
    {
        return Color(r * c.r, g * c.g, b * c.b);
    }

    Color operator+(const Color &c)
    {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

class Coef
{
public:
    double amb, diff, spec, refl;

    Coef()
    {
        amb = 0;
        diff = 0;
        spec = 0;
        refl = 0;
    }

    Coef(double amb, double diff, double spec, double refl)
    {
        this->amb = amb;
        this->diff = diff;
        this->spec = spec;
        this->refl = refl;
    }
};

class PointLight
{
public:
    PT position;
    Color color;

    PointLight(PT position, Color color)
    {
        this->position = position;
        this->color = color;
    }

    void draw()
    {
        glPointSize(10);
        glBegin(GL_POINTS);
        {
            glColor3f(color.r, color.g, color.b);
            glVertex3f(position.x, position.y, position.z);
        }
        glEnd();
        glBegin(GL_LINES);
        {
            glColor3f(color.r, color.g, color.b);
            glVertex3f(position.x, position.y, position.z);
            glVertex3f(position.x, position.y, 0);
        }
        glEnd();
    }
};

class SpotLight
{
public:
    PT position;
    PT direction;
    Color color;
    double cutOffAngle;

    SpotLight(PT position, PT direction, Color color, double cutOffAngle)
    {
        this->position = position;
        this->direction = getUnitVectorOf(direction);
        this->color = color;
        this->cutOffAngle = cutOffAngle;
    }

    void draw()
    {
        glPointSize(10);
        glBegin(GL_POINTS);
        {
            glColor3f(color.r, color.g, color.b);
            glVertex3f(position.x, position.y, position.z);
        }
        glEnd();
        glBegin(GL_LINES);
        {
            glColor3f(color.r, color.g, color.b);
            glVertex3f(position.x, position.y, position.z);
            glVertex3f(position.x, position.y, 0);

            // Spotlight direction
            glVertex3f(position.x, position.y, position.z);
            glVertex3f(position.x + 2*direction.x, position.y + 2*direction.y, position.z + 2*direction.z);
        }
        glEnd();
    }
};

class Ray
{
public:
    PT start;
    PT dir;

    Ray(PT start, PT dir)
    {
        this->start = start;
        this->dir = getUnitVectorOf(dir);
    }
};

class Object
{
protected:
    PT reference_point; // should have x, y, z
    double height, width, length;
    Color color;       // color of the object
    Coef coEfficients; // ambient, diffuse, specular, reflection coefficients
    int shine;         // exponent term of specular component

private:
    bool isBlocked(Ray L, Color &color, double distance)
    {
        double t = -1;
        for (Object *obj : objects)
        {
            t = obj->getT(L, color, 0);
            if (t > 0 and t + 0.000001 < distance)
            {
                return true;
            }
        }
        return false;
    }

    // Get angle between two vectors in degree
    double getAngle(PT vectA, PT vectB)
    {
        vectA = getUnitVectorOf(vectA);
        vectB = getUnitVectorOf(vectB);
        double dotProd = dotProduct(vectA, vectB);

        double theta = acos(dotProd) * (180.0 / M_PI); // In degree
        return fabs(theta);
    }

    void PointLightEffect(Color &color, Color colorAtIntersection, Ray ray, PT intersectionPoint)
    {
        for (PointLight *light : pointLights)
        {
            Ray L(light->position, intersectionPoint - light->position); // Light Ray
            PT N = getNormal(intersectionPoint, L);                      // Normalized (Plane Normal)

            double distance = modulusOf(intersectionPoint - light->position);

            // Check for the other light sources (Multiple light sources available)
            if (isBlocked(L, color, distance))
                continue;

            // labert = I_d * K_d * (N.L)
            double lambert = max(0.0, dotProduct(L.dir * (-1), N));

            // phong = I_s * K_s * (R.V)^k
            PT R = (N * dotProduct(L.dir, N) * 2) - L.dir; // Reflected Ray
            R = getUnitVectorOf(R);
            PT V = ray.dir * (-1.0);
            // ray is from eye to the object
            // L is from light source to object

            double phong = max(0.0, pow(dotProduct(V, R), shine));

            // Ambient co-efficient already applied to color
            color = color + (colorAtIntersection * (light->color * lambert * coEfficients.diff));
            color = color + (colorAtIntersection * (light->color * phong * coEfficients.spec));
        }
    }

    void SpotLightEffect(Color &color, Color colorAtIntersection, Ray ray, PT intersectionPoint)
    {
        for (SpotLight *light : spotLights)
        {
            Ray L(light->position, intersectionPoint - light->position); // Light Ray

            double beta = getAngle(L.dir, light->direction);

            // The object is in the hind side of the spotlight
            if (light->cutOffAngle < beta)
                continue;

            // Next the same calculation as the PointLight Source
            PT N = getNormal(intersectionPoint, L); // Normalized (Plane Normal)

            double distance = modulusOf(intersectionPoint - light->position);

            // Check for the other light sources (Multiple light sources available)
            if (isBlocked(L, color, distance))
                continue;

            // labert = I_d * K_d * (N.L)
            double lambert = max(0.0, dotProduct(L.dir * (-1), N));

            // phong = I_s * K_s * (R.V)^k
            PT R = (N * dotProduct(L.dir, N) * 2) - L.dir; // Reflected Ray
            R = getUnitVectorOf(R);
            PT V = getUnitVectorOf(ray.start - intersectionPoint);
            // ray is from eye to the object
            // L is from light source to object

            double phong = max(0.0, pow(dotProduct(V, R), shine));

            // Ambient co-efficient already applied to color
            color = color + (colorAtIntersection * (light->color * lambert * coEfficients.diff));
            color = color + (colorAtIntersection * (light->color * phong * coEfficients.spec));
        }
    }

public:
    Object() {}

    virtual void draw() {}

    void setColor(Color color)
    {
        this->color = color;
    }

    void setCoEfficients(Coef coef)
    {
        this->coEfficients = coef;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    virtual Color colorAtPoint(PT) = 0; // Pure virtual function

    // Would be overridden by the derived classes
    virtual PT getNormal(PT intersectionPoint, Ray ray) = 0; // Pure virtual function

    virtual double getT(Ray ray, Color color, int level) = 0; // Pure virtual function

    double getIntersection(Ray ray, Color &color, int level)
    {
        double t = getT(ray, color, level);

        if (level == 0)
            return t;
        if (t < 0)
            return -1; // The object is behind the eye
        // cout << "Level of recursion: " << levelOfRecursion << endl;

        PT intersectionPoint = ray.start + (ray.dir * t);

        // Update the color after reflection with ------- ambient coefficient --------
        Color colorAtIntersection = colorAtPoint(intersectionPoint); // Required for multilevel reflection (Before reflection)
        color = colorAtIntersection * coEfficients.amb;              // New color after reflection

        // Two types of light sources in action (Diffuse and Specular)
        PointLightEffect(color, colorAtIntersection, ray, intersectionPoint);
        SpotLightEffect(color, colorAtIntersection, ray, intersectionPoint);

        // RAY TRACING
        if (level >= levelOfRecursion)
            return t;

        PT normal = getNormal(intersectionPoint, ray);
        PT R_dir = ray.dir - (normal * dotProduct(ray.dir, normal) * 2);
        Ray R = Ray(intersectionPoint, R_dir);
        R.start = R.start + (R.dir * 0.00001); // Shift the origin a little to avoid self intersection

        Object *nearestObj = NULL;
        double t_min = INT_MAX;

        for (Object *obj : objects)
        {
            t = obj->getIntersection(R, color, 0); // Ray Casting
            // Jetar t shortest hobe, oita ke nearest hisebe nibo
            if (t > 0 and t < t_min)
            {
                t_min = t;
                nearestObj = obj;
            }
        }
        // Ray reflect hoye onno ekta object e porche (nearest)
        Color color_reflected(0, 0, 0);
        if (nearestObj != NULL)
        {
            t = nearestObj->getIntersection(R, color_reflected, level + 1);

            // color_reflected will be updated while in the subsequent call
            // update color using the impact of reflection
            color = color + (color_reflected * coEfficients.refl);
        }

        return t;
    }
};

class Floor : public Object
{
private:
    double tileWidth;

    bool isOutOfBoundary(PT p)
    {
        if (p.x < reference_point.x or p.x > reference_point.x + width)
            return true;
        if (p.y < reference_point.y or p.y > reference_point.y + width)
            return true;
        return false;
    }

public:
    Floor(double width, double tileWidth)
    {
        this->width = width;
        this->tileWidth = tileWidth;
        this->reference_point = PT(-width / 2, -width / 2, 0);
    }

    Color colorAtPoint(PT point) override
    {
        // get the tile number from point
        int tileCountX = (point.x - reference_point.x) / tileWidth;
        int tileCountY = (point.y - reference_point.y) / tileWidth;

        // The point is within the checker board
        if (!isOutOfBoundary(point))
        {
            if ((tileCountX + tileCountY) % 2 == 0)
            {
                return this->color;
            }
            else
            {
                return Color(0, 0, 0); // Black
            }
        }

        return Color(0, 0, 0);
    }

    PT getNormal(PT intersectionPoint, Ray incomingRay) override
    {
        return PT(0, 0, 1);
    }

    double getT(Ray ray, Color color, int level) override
    {
        PT planeNormal(0, 0, 1);
        double N_dot_R_0 = dotProduct(planeNormal, ray.dir);

        if (fabs(N_dot_R_0) <= 1e-5)
            return -1;

        double t = -dotProduct(planeNormal, ray.start - reference_point) / N_dot_R_0; // Assuming D = 0 (change === ray.start - reference_point)
        PT p = ray.start + ray.dir * t;

        if (isOutOfBoundary(p))
            return -1;

        return t;
    }

    void draw()
    {
        double half = width / 2;
        double tileCount = width / tileWidth;

        for (int i = 0; i < tileCount; i++)
        {
            for (int j = 0; j < tileCount; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3f(color.r, color.g, color.b); // Light RED
                }
                else
                {
                    glColor3f(0, 0, 0);
                }

                glBegin(GL_QUADS);
                {
                    glVertex3f(-half + i * tileWidth, -half + j * tileWidth, 0);
                    glVertex3f(-half + (i + 1) * tileWidth, -half + j * tileWidth, 0);
                    glVertex3f(-half + (i + 1) * tileWidth, -half + (j + 1) * tileWidth, 0);
                    glVertex3f(-half + i * tileWidth, -half + (j + 1) * tileWidth, 0);
                }
                glEnd();
            }
        }
    }
};

class Sphere : public Object
{
private:
    int stackCount, sectorCount;

    void drawASphereCut()
    {
        struct PT points[stackCount + 1][stackCount + 1];
        double x, y;

        // Transforming a cube side into a sphere cut
        for (int i = 0; i <= stackCount; i++)
        {
            x = -length + i * 1.0 / stackCount * (2 * length);

            for (int j = 0; j <= sectorCount; j++)
            {
                y = -length + j * 1.0 / sectorCount * (2 * length);

                points[i][j] = getUnitVectorOf(PT(x, y, length));
                points[i][j] = points[i][j] * length;
            }
        }

        for (int i = 0; i < stackCount; i++)
        {
            for (int j = 0; j < sectorCount; j++)
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

public:
    Sphere(PT center, double radius)
    {
        this->reference_point = center;
        this->length = radius;
        this->stackCount = 90;
        this->sectorCount = 90;
    }

    Color colorAtPoint(PT point) override
    {
        return this->color;
    }

    PT getNormal(PT intersectionPoint, Ray ray) override
    {
        PT normal = intersectionPoint - reference_point;
        return getUnitVectorOf(normal);
    }

    double getT(Ray ray, Color color, int level) override
    {
        PT R_0 = (ray.start - reference_point); // Sphere center at (0, 0, 0)
        PT R_d = ray.dir;

        double a = 1.0; // Modulus of R_d is 1
        double b = 2 * dotProduct(R_d, R_0);
        double c = dotProduct(R_0, R_0) - (length * length);

        double t1, t2;
        pair<double, double> sol = getQuadraticSolution(a, b, c);
        t1 = sol.first;
        t2 = sol.second;

        if (t1 < 0 and t2 < 0)
        {
            return -1;
        }

        double temp = t1;
        t1 = min(t1, t2);
        t2 = max(temp, t2);

        if (t1 > 0)
        {
            return t1;
        }

        if (t2 > 0)
        {
            return t2;
        }

        return -1;
    }

    void draw() override
    {
        glColor3f(color.r, color.g, color.b);
        for (int i = 0; i < 4; i++)
        {
            glPushMatrix();

            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            glRotatef(i * 90, 1, 0, 0);
            drawASphereCut();

            glPopMatrix();
        }

        for (int i = 0; i < 2; i++)
        {
            glPushMatrix();

            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            glRotatef(i * 180 + 90, 0, 1, 0);
            drawASphereCut();

            glPopMatrix();
        }
    }
};

class Triangle : public Object
{
private:
    PT a, b, c;
    double CrammerMatrix[3][3];

    double det(double mat[3][3])
    {
        double det = mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) - mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) + mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);

        return det;
    }

    void formMatrix(PT a, PT b, PT c) {
        CrammerMatrix[0][0] = a.x; CrammerMatrix[0][1] = b.x; CrammerMatrix[0][2] = c.x;
        CrammerMatrix[1][0] = a.y; CrammerMatrix[1][1] = b.y; CrammerMatrix[1][2] = c.y;
        CrammerMatrix[2][0] = a.z; CrammerMatrix[2][1] = b.z; CrammerMatrix[2][2] = c.z;
    }

public:
    Triangle(PT a, PT b, PT c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    Color colorAtPoint(PT point) override
    {
        return this->color;
    }

    PT getNormal(PT intersectionPoint, Ray incomingRay) override
    {
        PT normal = (b - a) * (c - a); // Cross product
        normal = getUnitVectorOf(normal);

        if (dotProduct(normal, incomingRay.dir) > 1e-5)
        {
            normal = normal * (-1.0);
        }
        return normal;
    }

    double getT(Ray ray, Color color, int level) override
    {
        // Intersection if beta + gamma < 1 and (beta, gamma) > 0 and t > 0
        PT R_0 = ray.start;
        PT R_d = ray.dir;
        double t = -1;

        formMatrix(a - b, a - c, R_d);
        double matA[3][3] = {{CrammerMatrix[0][0], CrammerMatrix[0][1], CrammerMatrix[0][2]},
                             {CrammerMatrix[1][0], CrammerMatrix[1][1], CrammerMatrix[1][2]},
                             {CrammerMatrix[2][0], CrammerMatrix[2][1], CrammerMatrix[2][2]}};
        double detValA = det(matA);
        // ------------------------------------------------ //

        formMatrix(a - R_0, a - c, R_d);
        double matB[3][3] = {{CrammerMatrix[0][0], CrammerMatrix[0][1], CrammerMatrix[0][2]},
                             {CrammerMatrix[1][0], CrammerMatrix[1][1], CrammerMatrix[1][2]},
                             {CrammerMatrix[2][0], CrammerMatrix[2][1], CrammerMatrix[2][2]}};
        double beta = det(matB) / detValA;

        formMatrix(a - b, a - R_0, R_d);
        double matG[3][3] = {{CrammerMatrix[0][0], CrammerMatrix[0][1], CrammerMatrix[0][2]},
                             {CrammerMatrix[1][0], CrammerMatrix[1][1], CrammerMatrix[1][2]},
                             {CrammerMatrix[2][0], CrammerMatrix[2][1], CrammerMatrix[2][2]}};
        double gamma = det(matG) / detValA;

        formMatrix(a - b, a - c, a - R_0);
        double matT[3][3] = {{CrammerMatrix[0][0], CrammerMatrix[0][1], CrammerMatrix[0][2]},
                             {CrammerMatrix[1][0], CrammerMatrix[1][1], CrammerMatrix[1][2]},
                             {CrammerMatrix[2][0], CrammerMatrix[2][1], CrammerMatrix[2][2]}};

        // If this condition is satisfied, then the ray intersects the triangle, else not
        if (beta + gamma < 1 and beta > 0 and gamma > 0)
        {
            t = det(matT) / detValA;
        }
        memset(CrammerMatrix, 0, sizeof(CrammerMatrix)); // Reset the matrix
        // clear memory

        if (t < 0)
            return -1; // The object is behind the eye (No intersection point found)

        return t;
    }

    void draw() override
    {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }
        glEnd();
    }
};

class General : public Object
{
private:
    double a, b, c, d, e, f, g, h, i, j;

    // 0 0 0 0 0 5	- cube reference point, length, width, height (0 indicates no clipping along this dimension)
    bool isOutOfBoundary(PT p)
    {
        if (length >= 0.00001 and (p.x < reference_point.x or p.x > reference_point.x + length))
            return true;
        if (width >= 0.00001 and (p.y < reference_point.y or p.y > reference_point.y + width))
            return true;
        if (height >= 0.00001 and (p.z < reference_point.z or p.z > reference_point.z + height))
            return true;

        return false;
    }

public:
    General(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, PT ref, double length, double width, double height)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
        this->h = h;
        this->i = i;
        this->j = j;
        this->reference_point = ref;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    Color colorAtPoint(PT point) override
    {
        return this->color; // Same color for the entire surface of the object
    }

    PT getNormal(PT intersectionPoint, Ray ray) override
    {
        // Gradient of the surface
        PT normal;
        normal.x = 2 * a * intersectionPoint.x + d * intersectionPoint.y + e * intersectionPoint.z + g;
        normal.y = 2 * b * intersectionPoint.y + d * intersectionPoint.x + f * intersectionPoint.z + h;
        normal.z = 2 * c * intersectionPoint.z + e * intersectionPoint.x + f * intersectionPoint.y + i;

        normal = getUnitVectorOf(normal);
        if (dotProduct(normal, ray.dir) > 1e-5)
        {
            normal = normal * (-1.0);
        }

        return normal;
    }

    double getT(Ray ray, Color color, int level) override
    {
        PT R_0 = ray.start;
        PT R_d = ray.dir;
        double t = -1;

        double A = a * R_d.x * R_d.x + b * R_d.y * R_d.y + c * R_d.z * R_d.z + d * R_d.x * R_d.y + e * R_d.x * R_d.z + f * R_d.y * R_d.z;
        double B = 2 * (a * R_0.x * R_d.x + b * R_0.y * R_d.y + c * R_0.z * R_d.z) + d * (R_0.x * R_d.y + R_0.y * R_d.x) + e * (R_0.x * R_d.z + R_0.z * R_d.x) + f * (R_0.y * R_d.z + R_0.z * R_d.y) + g * R_d.x + h * R_d.y + i * R_d.z;
        double C = a * R_0.x * R_0.x + b * R_0.y * R_0.y + c * R_0.z * R_0.z + d * R_0.x * R_0.y + e * R_0.x * R_0.z + f * R_0.y * R_0.z + g * R_0.x + h * R_0.y + i * R_0.z + j;
        // At^2 + Bt + C = 0 and A->0 then t = -C / B
        if (fabs(A) <= 0.00001)
        {
            return -C / B;
        }

        double t1, t2;
        pair<double, double> sol = getQuadraticSolution(A, B, C);
        t1 = sol.first;
        t2 = sol.second;

        if (t1 < 0 and t2 < 0)
        {
            return -1;
        }

        double temp = t1;
        t1 = min(t1, t2);
        t2 = max(temp, t2);

        if (t1 > 0)
        {
            if (!isOutOfBoundary(ray.start + ray.dir * t1))
                return t1;
        }

        if (t2 > 0)
        {
            if (!isOutOfBoundary(ray.start + ray.dir * t2))
                return t2;
        }

        return -1;
    }

    void draw() override
    {
        // Not implemented
    }
};

#endif