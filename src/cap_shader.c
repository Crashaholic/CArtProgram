#include "cap_shader.h"

#include <glad/glad.h>

FILE* Cap_OpenShaderFile(const char* loc)
{
	return Cap_OpenFile(loc, "r");
}

char* Cap_ShaderGetContentFromFile(const char* loc)
{
	return Cap_GetContentFromFile(loc, "r");
}

int Cap_SetupShaders(const char* vsFile, const char* fsFile, unsigned* vs, unsigned* fs, unsigned* sp)
{
    char* s = Cap_ShaderGetContentFromFile(vsFile);
    if (!s)
    {
        printf("Could not open VERTEX shader file!\n");
        return -1;
    }

    *vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(*vs, 1, &s, NULL);
    glCompileShader(*vs);
    {
        int  success;
        char infoLog[512] = { 0 };
        glGetShaderiv(*vs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(*vs, 512, NULL, infoLog);
            printf("Could not compile VERTEX shader: \n%s\n", infoLog);
        }
    }

    s = Cap_ShaderGetContentFromFile(fsFile);
    if (!s)
    {
        printf("Could not open FRAGMENT shader file!\n");
        return -1;
    }

    *fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fs, 1, &s, NULL);
    glCompileShader(*fs);
    {
        int  success;
        char infoLog[512] = { 0 };
        glGetShaderiv(*fs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(*fs, 512, NULL, infoLog);
            printf("Could not compile FRAGMENT shader: \n%s\n", infoLog);
            return -1;
        }
    }

    *sp = glCreateProgram();
    glAttachShader(*sp, *vs);
    glAttachShader(*sp, *fs);
    glLinkProgram(*sp);
    {
        int  success;
        char infoLog[512] = { 0 };
        glGetProgramiv(*sp, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(*sp, 512, NULL, infoLog);
            printf("Could not LINK SHADER: \n%s\n", infoLog);
            return -1;
        }
    }
    glDeleteShader(*vs);
    glDeleteShader(*fs);
    return 0;
}

void Cap_ShaderSetMat4(unsigned program, const char* uniform, float* p)
{
    glUniformMatrix4fv(glGetUniformLocation(program, uniform), 1, GL_FALSE, p);
}
