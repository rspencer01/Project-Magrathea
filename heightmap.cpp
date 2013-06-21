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

	
	float detail = (linearInterpolate(linearInterpolate(bd->detail,bd2->detail,fy),linearInterpolate(bd3->detail,bd4->detail,fy),fx));
	detail = 5*detail*changeContrast(getErosionData(float(x),float(y)),detail*detail);
	
	float terrain = (linearInterpolate(linearInterpolate(bd->topography,bd2->topography,fy),linearInterpolate(bd3->topography,bd4->topography,fy),fx));
	terrain = 140*1*changeContrast(getErosionData(float(x/10.f),float(y/10.f)),0.f);
	
	/*if (bd->flags & BIOME_FLAG_CRACKS)
		if (detail>4.8 && detail<5.4)
			detail = -detail;
	*/
	//float terrain = 50*getErosionData(float(x)/8,float(y)/8)*(linearInterpolate(linearInterpolate(bd->topography,bd2->topography,fy),linearInterpolate(bd3->topography,bd4->topography,fy),fx));
	float bias = 0*70*(cosineInterpolate(cosineInterpolate(bd->baseLevel,bd2->baseLevel,fy),cosineInterpolate(bd3->baseLevel,bd4->baseLevel,fy),fx));
	return  terrain + detail;
}
