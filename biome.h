#ifndef BIOME_H
#define BIOME_H

#include "magnathea.h"

#define BIOME_FLAG_CRACKS 1

#define CL_DESERT 1
#define CL_TUNDRA  2
#define CL_JUNGLE  3
#define CL_COASTAL 4

#define T_FREEZING 0.4
#define T_DESERT 0.3

struct biomeData
{
	unsigned char grassColor[3];
	float articness;
	float aridness;
	float landLevel;
	float baseLevel;
	float topography;
	float detail;
	float hilliness;
	float grassiness;
	float grassHeight;
	float moisture;
	float temperature;
	bool holes;
	int flags;
	int climate;
};

class biomeSystem
{
	private:
		biomeData** biomes;
		int sizeInMetres;
		int sizeInBiomes;
	public:
		int biomeSize;
		biomeSystem();
		biomeData* getAt(int,int);		
};

#endif