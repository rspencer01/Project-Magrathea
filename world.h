#ifndef WORLD_H
#define WORLD_H
class World;
#include "book.h"
#include "biome.h"
#include "region.h"
#include "heightmap.h"
#include "vector3.h"


#include <utility>
#include <list>

using namespace std;


class World
{
	private:
		int sizeOfVisibleSquareInRegions;
		int worldSize;
		int regionSize;
		list<list<region*> > visibleRegions;
		pair<int,int> centre;
		book* dataBook;
	public:
		heightMap* hm;
		World(int);
		int renderDetail;
		void Render(float,float);
		float cellHeight(int ,int );
		sbit* getSAt(int,int);
		biomeData* getBiomeAt(int,int);
		void Reset();
		void  setRenderDistance(float);
		float getRenderDistance();
		int getRegionSize();
};

#endif