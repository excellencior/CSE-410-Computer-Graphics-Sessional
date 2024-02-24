#ifndef LOOK_AT_CAMERA_H
#define LOOK_AT_CAMERA_H

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

#endif