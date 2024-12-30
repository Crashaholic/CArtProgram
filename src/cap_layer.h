#ifndef CAP_LAYER_H
#define CAP_LAYER_H

#include "cap_pixel.h"

typedef struct CAP_Layer
{
    CAP_PixelRGBA* data;
    unsigned width, height;
    unsigned textureId;
    int enabled;
} CAP_Layer;

typedef enum CAP_LAYER_BLEND_OPTIONS
{
    LBOP_NORMAL,
    LBOP_MULTIPLY,
    LBOP_ADDITIVE,
    LBOP_COUNT
}LAYER_BLEND_OPTIONS;

CAP_Layer Cap_CreateLayer(unsigned int width, unsigned int height);
void Cap_ReplaceLayer(CAP_Layer* target, unsigned int newWidth, unsigned int newHeight);
void Cap_RefreshLayerImage(CAP_Layer* layer);

#endif