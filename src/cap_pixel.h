#ifndef CAP_PIXEL_H
#define CAP_PIXEL_H

typedef struct CAP_PixelRGBA
{
    float r, g, b, a;
} CAP_PixelRGBA; // f32, GL_RGBA

typedef struct CAP_PixelRGB
{
    float r, g, b;
} CAP_PixelRGB; // f32, GL_RGB

#endif