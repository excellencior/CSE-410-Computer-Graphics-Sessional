#ifndef TRIANGLE_H
#define TRIANGLE_H

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

#endif