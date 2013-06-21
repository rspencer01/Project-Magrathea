#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <utility>
#include <string>
using namespace std;
#include "biome.h"

class heightMap
{
	public:
		biomeSystem* biomes;
		int sizeInMetres;
		heightMap();
		float* maxData;
		float getAt(int,int);
};




#endif