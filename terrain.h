#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glut.h>
#include "magrathea.h"
#include "vector3.h"
#include "biome.h"
#include "world.h"



#define TERRAIN_COUNT 7

#define TERRAIN_RAW_SIZE 512


void initTerrain();
GLuint getTerrainTexture(int);

#endif