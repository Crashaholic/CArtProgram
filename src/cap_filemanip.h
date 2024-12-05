#include <stdlib.h>
#include <stdio.h>

#ifndef CAP_FILEMANIP_H
#define CAP_FILEMANIP_H

// THIS SHOULD JUST READ FROM THE FILE!!!!!
FILE* Cap_OpenFile(const char* location, const char* mode);

int Cap_CloseFile(FILE* f);

char* Cap_GetContentFromFile(const char* location, const char* mode);

#endif
