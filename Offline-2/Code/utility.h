#ifndef UTILITY_H
#define UTILITY_H

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

// Utility class
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

#endif