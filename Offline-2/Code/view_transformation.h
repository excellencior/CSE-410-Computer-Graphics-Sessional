#ifndef VIEW_TRNSFORMATION_H
#define VIEW_TRNSFORMATION_H

vvd getViewTransformationMatrix(LookAtCamera EyeLookUp) {
    Utility util;
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

#endif