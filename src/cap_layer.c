#include "cap_layer.h"

#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>

CAP_Layer Cap_LayerCreate(unsigned int width, unsigned int height)
{
    CAP_Layer ret;
    
    ret.enabled = 1;
    //ret.data = calloc(width * height, sizeof(CAP_PixelRGBA));
    ret.data = malloc(width * height * sizeof(CAP_PixelRGBA));

    ret.width = width;
    ret.height = height;

    glGenTextures(1, &ret.textureId);
    glBindTexture(GL_TEXTURE_2D, ret.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret.width, ret.height, 0, GL_RGBA, GL_FLOAT, (void*)ret.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return ret;
}

void Cap_LayerRefreshImage(CAP_Layer* layer)
{
    glBindTexture(GL_TEXTURE_2D, layer->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layer->width, layer->height, 0, GL_RGBA, GL_FLOAT, (void*)layer->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Cap_LayerReplace(CAP_Layer* target, unsigned int newWidth, unsigned int newHeight)
{
    CAP_PixelRGBA* temp = malloc(newWidth * newHeight * sizeof(CAP_PixelRGBA));
    if (!temp)
    {
        fprintf(stderr, "could not allocate for Cap_LayerReplace");
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
