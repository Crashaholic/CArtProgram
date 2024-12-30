#ifndef CAP_FILEMANIP_H
#define CAP_FILEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <nfd.h>

typedef struct CAP_PixelRGBA CAP_PixelRGBA;

// THIS SHOULD JUST READ FROM THE FILE!!!!!
FILE* Cap_OpenFile(const char* location, const char* mode);

int Cap_CloseFile(FILE* f);

char* Cap_GetContentFromFile(const char* location, const char* mode);

void Cap_WriteCanvasToFile(const char* location, CAP_PixelRGBA* data, unsigned width, unsigned height);

#endif
