#ifndef MAGRATHEA_H
#define MAGRATHEA_H

// An ill named header class that contains things everyone shoud know
#include "vector3.h"
#include <algorithm>
#include <GL/glut.h>

#define WORLD_SIZE (8196)

const GLfloat skyColor[4]= {0.63671875f, 0.94140625f, 1.f, 1.0f}; 

//Surface bit  No classes so deletion is faster
struct sbit
{
	int x;
	int y;
	float elevation;
	float waterLevel;
	float normal [3];
	float colour [3];
	bool isGrass;
	float grassHeight;
	int surfaceType;
};


#define SURFACE_WATER 0
#define SURFACE_ROCK  1
#define SURFACE_SAND  2
#define SURFACE_DIRT  3
#define SURFACE_SNOW  4
#define SURFACE_GRASS 5
#define SURFACE_BUSH  6



#endif