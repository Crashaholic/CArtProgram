#ifndef CAP_PIXEL_H
#define CAP_PIXEL_H

#include <stdint.h>

typedef struct
{
    float r, g, b, a;
} CAP_PixelRGBA; // f32, GL_RGBA

typedef struct
{
    float r, g, b;
} CAP_PixelRGB; // f32, GL_RGB

#endif