#include "cap_math.h"

float lerp(float a, float b, float f)
{
    return a * (1.0f - f) + (b * f);
}

float degToRad(float ipt)
{
    return ipt * (float)M_PI / 180.f;
}

float radToDeg(float ipt)
{
    return ipt * 180.f / (float)M_PI;
}
