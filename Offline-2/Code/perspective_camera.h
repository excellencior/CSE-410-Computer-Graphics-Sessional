#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

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

#endif