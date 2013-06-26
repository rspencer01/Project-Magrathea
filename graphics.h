#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glut.h>
#include <stdarg.h>
#include "vector3.h"


Vector3 getNormal3f(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3);
void drawFPS();
void resetStats();
void addTriangles(int);
void displayHUD();
void toggleHUD();
void writeString(int, int,const char* format, ... );
void RenderCanvasBegin (int left, int right, int bottom, int top, int size);
void RenderCanvasEnd();
#endif
