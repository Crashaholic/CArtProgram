#include "cap_layer.h"

#include <stdlib.h>
#include <stdio.h>

CAP_Layer Cap_CreateLayer(unsigned int width, unsigned int height)
{
    CAP_Layer ret;
    
    ret.enabled = 1;
    //ret.data = calloc(width * height, sizeof(CAP_PixelRGBA));
    ret.data = malloc(width * height * sizeof(CAP_PixelRGBA));

    ret.width = width;
    ret.height = height;

    return ret;
}

void Cap_ReplaceLayer(CAP_Layer* target, unsigned int newWidth, unsigned int newHeight)
{
    CAP_PixelRGBA* temp = malloc(newWidth * newHeight * sizeof(CAP_PixelRGBA));
    if (!temp)
    {
        fprintf(stderr, "could not allocate for Cap_ReplaceLayer");
        return;
    }
    int x = 0, y = 0;
    /*for (y = 0; y < target->height; ++y)
    {
        for (x = 0; x < target->width; ++x)
        {
            temp[target->width * y + x].r = target->data[target->width * y + x].r;
            temp[target->width * y + x].g = target->data[target->width * y + x].g;
            temp[target->width * y + x].b = target->data[target->width * y + x].b;
            temp[target->width * y + x].a = target->data[target->width * y + x].a;
        }
    }*/
    free(target->data);
    target->data = temp;

    target->width = newWidth;
    target->height = newHeight;
}
