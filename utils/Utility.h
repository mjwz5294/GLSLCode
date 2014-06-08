#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "Dependencies.h"
#include "RenderSystem.h"

extern void renderTexture2D(GLuint texture_id);

extern int LoadGLTextures(char *filename);
extern int LoadGLTexturesCube();
extern int LoadGLTextureBmp(char *file);

#endif