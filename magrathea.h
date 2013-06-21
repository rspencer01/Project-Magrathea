#ifndef MAGRATHEA_H
#define MAGRATHEA_H

// An ill named header class that contains things everyone shoud know
#include "vector3.h"
#include <algorithm>
#include <GL/glut.h>

#define WORLD_SIZE (1024)

const GLfloat skyColor[4]= {0.63671875f, 0.94140625f, 1.f, 1.0f}; 

//Surface bit
struct sbit
{
	int x;
	int y;
	float elevation;
	float waterLevel;
	Vector3* normal;
	Vector3* colour;
	bool isGrass;
	float grassHeight;
	int surfaceType;
};


#define SURFACE_GRASS 0
#define SURFACE_DIRT  1
#define SURFACE_WATER 2
#define SURFACE_ROCK  3
#define SURFACE_BUSH  4
#define SURFACE_SAND  5
#define SURFACE_SNOW  6


#endif