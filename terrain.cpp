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


int getPixelAt(int rgb,int terrainNumber,int i,int j,float* colour, bool doCol)
{
	if (! doCol)
		return textureData[terrainNumber][repIndex(i,j+1)*3 + rgb];
	return textureData[terrainNumber][repIndex(i,j+1)*3 + rgb] * colour[rgb]/255;
}

GLuint MakeCompositeTerrain(int size,World* parent,int detail,int X,int Y)
{
	detail = detail<1?1:detail>13?13:detail;
	int TERRAIN_OUTPUT_TEXTURE_SIZE = 1<<detail;
	unsigned char* finalRGB = new unsigned char[TERRAIN_OUTPUT_TEXTURE_SIZE*TERRAIN_OUTPUT_TEXTURE_SIZE*3];
	//Lets rewrite this, shall we?
	//First clear the slate
	memset(finalRGB,100,TERRAIN_OUTPUT_TEXTURE_SIZE*TERRAIN_OUTPUT_TEXTURE_SIZE*3);
	//Now, iterate over all the points in this area
	int splotSize = TERRAIN_OUTPUT_TEXTURE_SIZE/(size*2);

	for (int y = Y;y<Y+size;y++)
		for (int x = X;x<X+size;x++)
		{
			int terrX = (x-X)*TERRAIN_OUTPUT_TEXTURE_SIZE/size;
			int terrY = (y-Y)*TERRAIN_OUTPUT_TEXTURE_SIZE/size;
			sbit* here = parent->getSAt(y,x);
			for (int dy = max(0,terrY-splotSize);dy<min(TERRAIN_OUTPUT_TEXTURE_SIZE,terrY+splotSize);dy++)
				for (int dx = max(0,terrX-splotSize);dx<min(TERRAIN_OUTPUT_TEXTURE_SIZE,terrX+splotSize);dx++)
				{
					int textX = (dx-(terrX-splotSize))*TERRAIN_RAW_SIZE/TERRAIN_OUTPUT_TEXTURE_SIZE;
					int textY = (dy-(terrY-splotSize))*TERRAIN_RAW_SIZE/TERRAIN_OUTPUT_TEXTURE_SIZE;
					finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3] = textureData[here->surfaceType][(textY*TERRAIN_RAW_SIZE+textX)*3];
					finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1] = textureData[here->surfaceType][(textY*TERRAIN_RAW_SIZE+textX)*3+1];
					finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2] = textureData[here->surfaceType][(textY*TERRAIN_RAW_SIZE+textX)*3+2];
					if (here->surfaceType == SURFACE_GRASS)
					{
						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3] = finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3]*here->colour[0]/255;
						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1] = finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1]*here->colour[1]/255;
						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2] = finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2]*here->colour[2]/255;
					}
				}
				

		}
	GLuint TextureNumber = LoadTextureFromData(finalRGB,TERRAIN_OUTPUT_TEXTURE_SIZE,TERRAIN_OUTPUT_TEXTURE_SIZE);
	delete finalRGB;
	return TextureNumber;
}
