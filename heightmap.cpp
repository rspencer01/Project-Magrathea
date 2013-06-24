#include "magrathea.h"
#include "heightmap.h"
#include "misc.h"
#include "noise.h"
#include "erosion.h"
#include "biome.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <stdio.h>
#include <fstream>


// All measurements are in metres
// We will make this about the size of South Africa.
#define SCALE 0.1
#define ROUND(_a,_b) (int(_b)*(int(_a)/int(_b)))
#define REGION_SIZE 63


inline float min(float a, float b)
{
	return a>b?b:a;
}


heightMap::heightMap()
{
	biomes = new biomeSystem();
}

float ci(float a,float b, float x)
{
	float f = (1-cos(x*3.1415))/2.0;
	return a*(1-f)+b*f;
}

float heightMap::getAt(int y,int x)
{
	
	int bs = biomes->biomeSize;
	assert(y>=0 && y<=WORLD_SIZE);
	assert(x>=0 && x<=WORLD_SIZE);
	biomeData* bd = biomes->getAt(x,y);
	biomeData* bd2 = biomes->getAt(x,y+bs);
	biomeData* bd3 = biomes->getAt(x+bs,y);
	biomeData* bd4 = biomes->getAt(x+bs,y+bs);
	float fx = (x%bs)/float(bs);
	float fy = (y%bs)/float(bs);

	

	float detail = 2*getErosionData(float(x*2.f),float(y*2.f));

	if (bd->flags & FORM_BUTTE)
		if (detail >1.2)
			detail = 2+detail;
	if (bd->flags & FORM_CRATER)
		if (detail >1.0)
			detail = 1.2-detail;


	float terrainMultiplyer = (linearInterpolate(linearInterpolate(bd->topography,bd2->topography,fy),linearInterpolate(bd3->topography,bd4->topography,fy),fx));
	float terrain = 45*terrainMultiplyer*getErosionData(float(x/5.f),float(y/5.f));
	float bias = 70*(cosineInterpolate(cosineInterpolate(bd->baseLevel,bd2->baseLevel,fy),cosineInterpolate(bd3->baseLevel,bd4->baseLevel,fy),fx));
	return  terrain + detail + bias;
}
