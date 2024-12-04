#ifndef CAP_MATH_H
#define CAP_MATH_H



float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}


#endif
