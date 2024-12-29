#include "cap_layer.h"

#include <stdlib.h>

CAP_Layer CreateLayer(unsigned int width, unsigned int height)
{
    CAP_Layer ret;
    
    ret.enabled = 1;
    //ret.data = calloc(width * height, sizeof(CAP_PixelRGBA));
    ret.data = malloc(width * height * sizeof(CAP_PixelRGBA));

    ret.width = width;
    ret.height = height;

    return ret;
}
