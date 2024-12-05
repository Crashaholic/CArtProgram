#include "cap_shader.h"

FILE* Cap_OpenShaderFile(const char* loc)
{
	return Cap_OpenFile(loc, "r");
}

char* Cap_ShaderGetContentFromFile(const char* loc)
{
	return Cap_GetContentFromFile(loc, "r");
}
