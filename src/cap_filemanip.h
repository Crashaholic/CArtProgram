#ifndef CAP_FILEMANIP_H
#define CAP_FILEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <nfd.h>

typedef struct CAP_PixelRGBA CAP_PixelRGBA;
typedef struct CAP_Layer CAP_Layer;

// THIS SHOULD JUST READ FROM THE FILE!!!!!
FILE* Cap_FileIOOpen(const char* location, const char* mode);
int Cap_FileIOClose(FILE* f);
char* Cap_FileIOGetContent(const char* location, const char* mode);
void Cap_FileIOWriteCanvas(const char* location, CAP_PixelRGBA* data, unsigned width, unsigned height);
void Cap_FileIOExportFromLayer(CAP_Layer* layer);
void Cap_FileIOImportToLayer(CAP_Layer* layer);

#endif
