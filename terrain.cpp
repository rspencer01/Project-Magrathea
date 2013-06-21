#include "textures.h"
#include "terrain.h"
#include "misc.h"
#include "biome.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glut.h>
using namespace std;
const char*    textureNames[TERRAIN_COUNT] = {"sgrassa512.raw","sdirt512.raw","swater512.raw","srock512.raw","sgrassb512.raw","ssand512.raw","ssnow512.raw"};
unsigned char* textureData[TERRAIN_COUNT];

void initTerrain()
{
	for (int i = 0;i<TERRAIN_COUNT;i++)
		textureData[i] = loadRaw(textureNames[i],TERRAIN_RAW_SIZE);
}

inline float linearInter(float x1,float x2,float t)
{
	return x1*t+x2*(1.0-t);
}

inline int repIndex(int a,int b)
{
	return (a%TERRAIN_RAW_SIZE)*TERRAIN_RAW_SIZE + (b%TERRAIN_RAW_SIZE);
}


GLuint MakeCompositeTerrain(int size,World* parent,int detail,int X,int Y)
{
	detail = detail<1?1:detail>10?10:detail;
	int TERRAIN_OUTPUT_TEXTURE_SIZE = 1<<detail;
	unsigned char* finalRGB = new unsigned char[TERRAIN_OUTPUT_TEXTURE_SIZE*TERRAIN_OUTPUT_TEXTURE_SIZE*3];
	//Terrains+Normals
	float** vectorSpace = new float*[TERRAIN_COUNT+1+3];
	for (int i = 0;i<TERRAIN_COUNT+1+3;i++)
		vectorSpace[i] = new float[size*size];
	for (int i = 0;i<TERRAIN_OUTPUT_TEXTURE_SIZE;i++)
	{
		float shadowHeight = -10000.0;
		for (int j = TERRAIN_OUTPUT_TEXTURE_SIZE-1;j>=0;j--)
		{
			int y = Y+(i*size/TERRAIN_OUTPUT_TEXTURE_SIZE);
			int x = X+(j*size/TERRAIN_OUTPUT_TEXTURE_SIZE);
			
			int terrainNumber = parent->getSAt(y,x)->surfaceType;
			float lighting = Vector3(1.0/1.414213,1.0/1.414213,0).dot(*(parent->getSAt(y,x)->normal));
			
			int r = textureData[terrainNumber][repIndex(i,j)*3];
			int g = textureData[terrainNumber][repIndex(i,j)*3 + 1];
			int b = textureData[terrainNumber][repIndex(i,j)*3 + 2];
			if (terrainNumber == SURFACE_GRASS)
			{
				r = r * parent->getSAt(y,x)->colour->x / 256;
				g = g * parent->getSAt(y,x)->colour->y / 256;
				b = b * parent->getSAt(y,x)->colour->z / 256;
			}
			
			
			/*
			if (shadowHeight > parent->getSAt(y,x)->elevation)
			{
				lighting = 0.0;
			}
			else
			{
				shadowHeight=parent->getSAt(y,x)->elevation;
			}
			shadowHeight-= 1.0 *float(size)/TERRAIN_OUTPUT_TEXTURE_SIZE;
			*/
			r = max(min(r*lighting,255.f),0.f);
			g = max(min(g*lighting,255.f),0.f);
			b = max(min(b*lighting,255.f),0.f);
			finalRGB[(i*TERRAIN_OUTPUT_TEXTURE_SIZE+j)*3]   = r;
			finalRGB[(i*TERRAIN_OUTPUT_TEXTURE_SIZE+j)*3+1] = g;
			finalRGB[(i*TERRAIN_OUTPUT_TEXTURE_SIZE+j)*3+2] = b;
		}
	}
	GLuint TextureNumber = LoadTextureFromData(finalRGB,TERRAIN_OUTPUT_TEXTURE_SIZE,TERRAIN_OUTPUT_TEXTURE_SIZE);
	delete finalRGB;
	for (int i = 0;i<TERRAIN_COUNT+1;i++)
		delete vectorSpace[i];
	delete vectorSpace;
	return TextureNumber;
}
