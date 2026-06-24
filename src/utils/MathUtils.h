#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QtMath>

inline double normalizeAngle180(double deg) {
    deg = fmod(deg, 360.0);
    if (deg > 180.0)  deg -= 360.0;
    if (deg < -180.0) deg += 360.0;
    return deg;
}

#endif
