#include "cap_filemanip.h"
#include "cap_pixel.h"
#include <math.h> // For rounding
#include "stb_image_write.h"

// THIS SHOULD JUST READ FROM THE FILE!!!!!
FILE* Cap_OpenFile(const char* location, const char* mode)
{
	FILE* temp = fopen(location, "r");
	return temp;
}

int Cap_CloseFile(FILE* f)
{
	return fclose(f);
}

char* Cap_GetContentFromFile(const char* location, const char* mode)
{
    FILE* f = Cap_OpenFile(location, mode);
    char* ret = 0;
    if (f)
    {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        if (fsize == -1L)
        {
            fprintf(stderr, "Error: Could not determine file size\n");
            Cap_CloseFile(f);
            return NULL;
        }
        fseek(f, 0, SEEK_SET);

        char* ret = calloc(fsize + 1, 1);
        if (!ret)
        {
            fprintf(stderr, "Memory allocation failed\n");
            Cap_CloseFile(f);
            return NULL;
        }
        else
        {
            fread(ret, 1, fsize, f);
            Cap_CloseFile(f);
            ret[fsize] = '\0';
            return ret;
        }
    }
    return ret;
}

void Cap_WriteCanvasToFile(const char* location, CAP_PixelRGBA* data, unsigned width, unsigned height) 
{
    unsigned char* imageData = (unsigned char*)malloc(width * height * 4);
    if (!imageData) {
        fprintf(stderr, "Memory allocation failed!\n");
        return;
    }

    for (unsigned i = 0; i < width * height; ++i) {
        imageData[i * 4 + 0] = (unsigned char)(fminf(fmaxf(data[i].r, 0.0f), 1.0f) * 255.0f);
        imageData[i * 4 + 1] = (unsigned char)(fminf(fmaxf(data[i].g, 0.0f), 1.0f) * 255.0f);
        imageData[i * 4 + 2] = (unsigned char)(fminf(fmaxf(data[i].b, 0.0f), 1.0f) * 255.0f);
        imageData[i * 4 + 3] = (unsigned char)(fminf(fmaxf(data[i].a, 0.0f), 1.0f) * 255.0f);
    }

    if (!stbi_write_png(location, width, height, 4, imageData, width * 4)) {
        fprintf(stderr, "Failed to write PNG!\n");
    }

    free(imageData);
}
