#include "cap_filemanip.h"

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
