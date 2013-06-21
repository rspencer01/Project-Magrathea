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


int getPixelAt(int rgb,int terrainNumber,int i,int j,Vector3* colour, bool doCol)
{
	if (! doCol)
		return textureData[terrainNumber][repIndex(i,j+1)*3 + rgb];
	return textureData[terrainNumber][repIndex(i,j+1)*3 + rgb] * colour->getDimension(rgb) / 256;
}

GLuint MakeCompositeTerrain(int size,World* parent,int detail,int X,int Y)
{
	detail = detail<1?1:detail>13?13:detail;
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
			float fx = X+(float(j)*size/TERRAIN_OUTPUT_TEXTURE_SIZE) - x;
			float fy = Y+(float(i)*size/TERRAIN_OUTPUT_TEXTURE_SIZE) - y;
			
			int terrainNumber = parent->getSAt(y,x)->surfaceType;
			int terrainNumber1 = parent->getSAt(y,x+1)->surfaceType;
			int terrainNumber2 = parent->getSAt(y+1,x)->surfaceType;
			int terrainNumber3 = parent->getSAt(y+1,x+1)->surfaceType;
			float lighting = Vector3(1.0/1.414213,1.0/1.414213,0).dot(*(parent->getSAt(y,x)->normal));
			
			int r1 = getPixelAt(0,terrainNumber,i,j,parent->getSAt(y,x)->colour,terrainNumber==SURFACE_GRASS);
			int g1 = getPixelAt(1,terrainNumber,i,j,parent->getSAt(y,x)->colour,terrainNumber==SURFACE_GRASS);
			int b1 = getPixelAt(2,terrainNumber,i,j,parent->getSAt(y,x)->colour,terrainNumber==SURFACE_GRASS);
			int r2 = getPixelAt(0,terrainNumber1,i,j,parent->getSAt(y,x+1)->colour,terrainNumber1==SURFACE_GRASS);
			int g2 = getPixelAt(1,terrainNumber1,i,j,parent->getSAt(y,x+1)->colour,terrainNumber1==SURFACE_GRASS);
			int b2 = getPixelAt(2,terrainNumber1,i,j,parent->getSAt(y,x+1)->colour,terrainNumber1==SURFACE_GRASS);
			int r3 = getPixelAt(0,terrainNumber2,i,j,parent->getSAt(y+1,x)->colour,terrainNumber2==SURFACE_GRASS);
			int g3 = getPixelAt(1,terrainNumber2,i,j,parent->getSAt(y+1,x)->colour,terrainNumber2==SURFACE_GRASS);
			int b3 = getPixelAt(2,terrainNumber2,i,j,parent->getSAt(y+1,x)->colour,terrainNumber2==SURFACE_GRASS);
			int r4 = getPixelAt(0,terrainNumber3,i,j,parent->getSAt(y+1,x+1)->colour,terrainNumber3==SURFACE_GRASS);
			int g4 = getPixelAt(1,terrainNumber3,i,j,parent->getSAt(y+1,x+1)->colour,terrainNumber3==SURFACE_GRASS);
			int b4 = getPixelAt(2,terrainNumber3,i,j,parent->getSAt(y+1,x+1)->colour,terrainNumber3==SURFACE_GRASS);


			int r = linearInterpolate(linearInterpolate(r1,r2,fx),linearInterpolate(r3,r4,fx),fy);
			int g = linearInterpolate(linearInterpolate(g1,g2,fx),linearInterpolate(g3,g4,fx),fy);
			int b = linearInterpolate(linearInterpolate(b1,b2,fx),linearInterpolate(b3,b4,fx),fy);

			
			
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
