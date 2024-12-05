#include "cap_math.h"

float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

float degToRad(float ipt)
{
    return ipt * M_PI / 180.f;
}

float radToDeg(float ipt)
{
    return ipt * 180.f / M_PI;
}
