#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

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

#endif