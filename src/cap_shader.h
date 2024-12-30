#ifndef CAP_SHADER_H
#define CAP_SHADER_H

#include "cap_filemanip.h"

FILE* Cap_ShaderOpenFile(const char* loc);
char* Cap_ShaderGetContentFromFile(const char* loc);
int Cap_ShaderSetup(const char* vsFile, const char* fsFile, unsigned* vs, unsigned* fs, unsigned* sp);

void Cap_ShaderSetMat4(unsigned program, const char* uniform, float* p);

#endif