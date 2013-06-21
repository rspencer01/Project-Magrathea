#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glut.h>
#include "magnathea.h"
#include "vector3.h"
#include "biome.h"
#include "world.h"



#define TERRAIN_COUNT 7

#define TERRAIN_RAW_SIZE 512

struct terrainBit
{
	int x,y;
	float  shade;
	
	float grassMult[3];
};


void initTerrain();
GLuint MakeCompositeTerrain(int size,World* parent,int detail,int x,int y);


#endif