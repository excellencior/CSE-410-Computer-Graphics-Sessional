#ifndef PROJECTION_TRANSFORMATION_H
#define PROJECTION_TRANSFORMATION_H

vvd getProjectionTransformationMatrix(PerspectiveCamera camera) {
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

#endif