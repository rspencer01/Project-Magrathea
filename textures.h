#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glut.h>


GLuint LoadTextureFromData(unsigned char*,int,int);
GLuint LoadAlphaTextureFromData(unsigned char*,int,int);
GLuint LoadTextureFromFile(const char*,int,int);

#endif