#include "textures.h"
#include "terrain.h"
#include "misc.h"
#include "biome.h"
#include "graphics.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glut.h>

using namespace std;
const char*    textureNames[TERRAIN_COUNT] = {"swater512.raw","srock512.raw","ssand512.raw","sdirt512.raw","ssnow512.raw","solidGrass512.raw","sgrassb512.raw"};
unsigned char* textureData[TERRAIN_COUNT];
GLuint textureIds[TERRAIN_COUNT];

void initTerrain()
{
	for (int i = 0;i<TERRAIN_COUNT;i++)
	{
		textureData[i] = loadRaw(textureNames[i],TERRAIN_RAW_SIZE);
		textureIds[i] = LoadTextureFromFile(textureNames[i],TERRAIN_RAW_SIZE,TERRAIN_RAW_SIZE);
	}
}

inline float linearInter(float x1,float x2,float t)
{
	return x1*t+x2*(1.0-t);
}

int repIndex(int a,int b,int c)
{
	if (c%2==0)
	{
		int d = a;
		a = b;
		b = d;
	}
	return ((a+TERRAIN_RAW_SIZE)%TERRAIN_RAW_SIZE)*TERRAIN_RAW_SIZE + ((b+TERRAIN_RAW_SIZE)%TERRAIN_RAW_SIZE);
}

GLuint getTerrainTexture(int index)
{
	return textureIds[index];
}



