#include "textures.h"
#include "terrain.h"
#include "misc.h"
#include "biome.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glut.h>
using namespace std;
const char*    textureNames[TERRAIN_COUNT] = {"swater512.raw","srock512.raw","ssand512.raw","sdirt512.raw","ssnow512.raw","solidGrass512.raw","sgrassb512.raw"};
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

GLuint MakeCompositeTerrain(int size,World* parent,int detail,int X,int Y)
{
	detail = detail<1?1:detail>13?13:detail;
	int TERRAIN_OUTPUT_TEXTURE_SIZE = 1<<detail;
	unsigned char* finalRGB = new unsigned char[TERRAIN_OUTPUT_TEXTURE_SIZE*TERRAIN_OUTPUT_TEXTURE_SIZE*3];
	//Lets rewrite this, shall we?
	//First clear the slate
	memset(finalRGB,100,TERRAIN_OUTPUT_TEXTURE_SIZE*TERRAIN_OUTPUT_TEXTURE_SIZE*3);
	//Now, iterate over all the points in this area
	int splotSize = TERRAIN_OUTPUT_TEXTURE_SIZE/(size*2)*1.9;

	vector<pair<int,int> > orders [TERRAIN_COUNT];
	for (int y = Y;y<Y+size;y++)
		for (int x = X;x<X+size;x++)
		{
			sbit* here = parent->getSAt(y,x);
			orders[here->surfaceType].push_back(make_pair(y,x));
		}
	for (int terrainNumber = 0;terrainNumber<TERRAIN_COUNT;terrainNumber++)
		while (! orders[terrainNumber].empty())
		{
			int y = orders[terrainNumber].back().first;
			int x = orders[terrainNumber].back().second;
			orders[terrainNumber].pop_back();
			int terrX = (x-X)*TERRAIN_OUTPUT_TEXTURE_SIZE/size;
			int terrY = (y-Y)*TERRAIN_OUTPUT_TEXTURE_SIZE/size;
			sbit* here = parent->getSAt(y,x);
			int transform = rand();
			for (int dy = max(0,terrY-splotSize);dy<min(TERRAIN_OUTPUT_TEXTURE_SIZE,terrY+splotSize);dy++)
				for (int dx = max(0,terrX-splotSize);dx<min(TERRAIN_OUTPUT_TEXTURE_SIZE,terrX+splotSize);dx++)
				{
					int textX = (dx-(terrX-splotSize))*TERRAIN_RAW_SIZE/(splotSize*2);
					int textY = (dy-(terrY-splotSize))*TERRAIN_RAW_SIZE/(splotSize*2);
					//Transparent colour is black
					if ( textureData[terrainNumber][repIndex(textY,textX,transform)*3] +
						 textureData[terrainNumber][repIndex(textY,textX,transform)*3+1] +
						 textureData[terrainNumber][repIndex(textY,textX,transform)*3+2]  != 0)
					{
						float shader = max(0.1f,Vector3(0.707107,0.707107,0).dot(Vector3(here->normal)));


						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3]   = textureData[terrainNumber][repIndex(textY,textX,transform)*3] * shader;
						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1] = textureData[terrainNumber][repIndex(textY,textX,transform)*3+1] * shader;
						finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2] = textureData[terrainNumber][repIndex(textY,textX,transform)*3+2] * shader;
						if (here->surfaceType == SURFACE_GRASS)
						{
							finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3] = max(0.f,finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3]*here->colour[0]/255);
							finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1] = max(0.f,finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+1]*here->colour[1]/255);
							finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2] = max(0.f,finalRGB[(dy*TERRAIN_OUTPUT_TEXTURE_SIZE+dx)*3+2]*here->colour[2]/255);
						}
					}
				}
		}
	GLuint TextureNumber = LoadTextureFromData(finalRGB,TERRAIN_OUTPUT_TEXTURE_SIZE,TERRAIN_OUTPUT_TEXTURE_SIZE);
	delete finalRGB;
	return TextureNumber;
}
