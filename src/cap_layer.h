#ifndef CAP_LAYER_H
#define CAP_LAYER_H

#include "cap_pixel.h"

typedef struct
{
    CAP_PixelRGBA* data;
    unsigned width, height;
    int enabled;
} CAP_Layer ;

typedef enum
{
    LBOP_NORMAL,
    LBOP_MULTIPLY,
    LBOP_ADDITIVE,
    LBOP_COUNT
}LAYER_BLEND_OPTIONS;

CAP_Layer CreateLayer(unsigned int width, unsigned int height);

#endif