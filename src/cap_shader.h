#ifndef CAP_SHADER_H
#define CAP_SHADER_H

#include "cap_filemanip.h"

FILE* Cap_OpenShaderFile(const char* loc);

char* Cap_ShaderGetContentFromFile(const char* loc);


#endif