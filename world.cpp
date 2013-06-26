#include "misc.h"
#include "world.h"
#include "region.h"
#include "graphics.h"
#include "erosion.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <utility>
#include <assert.h>
#include <list>
#include <algorithm>
using namespace std;

#define ROUND(_a,_b) (int(_b)*(int(_a)/int(_b)))
#define REGION_SIZE 32

World::World(int sizeInMetres)
{
	initErosion();
	worldSize = sizeInMetres;
	regionSize = REGION_SIZE;
	sizeOfVisibleSquareInRegions = 1;
	hm = new heightMap();
	dataBook = new book(this);
	renderDetail = 5;
}
//The render function.  A hell of a thing.  Takes up too much code.  Sort it out.
void World::Render(float x, float y)
{

	int regionXIndex = (int)x/regionSize;
	int regionYIndex = (int)y/regionSize;
	
	if (visibleRegions.size()==0)
	{
		list<region*> newRow;
		newRow.push_back(new region(regionSize+1,regionXIndex*regionSize,regionYIndex*regionSize,this));
		visibleRegions.push_back(newRow);
	}
	// CREATION OF NEW REGIONS
	for (list<list<region*> >::iterator it = visibleRegions.begin();it!=visibleRegions.end();it++)
	{
		if (((*it).front()->origin_x > (regionXIndex-sizeOfVisibleSquareInRegions/2)*regionSize) && ((*it).front()->origin_x-regionSize>0))
			(*it).push_front(new region(regionSize+1,(*it).front()->origin_x-regionSize,(*it).front()->origin_y,this));
		if (((*it).back()->origin_x  < (regionXIndex+sizeOfVisibleSquareInRegions/2)*regionSize) && ((*it).back()->origin_x+regionSize<WORLD_SIZE-regionSize))
			(*it).push_back(new region(regionSize+1,(*it).back()->origin_x+regionSize,(*it).front()->origin_y,this));
	}
	
	if ((visibleRegions.front().front()->origin_y > (regionYIndex-sizeOfVisibleSquareInRegions/2)*regionSize) && (visibleRegions.front().front()->origin_y-regionSize > 0))
	{
		list<region*> newRow;
		newRow.push_back(new region(regionSize+1,visibleRegions.front().front()->origin_x,visibleRegions.front().front()->origin_y-regionSize,this));
		visibleRegions.push_front(newRow);
	}
	
	if ((visibleRegions.back().front()->origin_y < (regionYIndex+sizeOfVisibleSquareInRegions/2)*regionSize) && (visibleRegions.back().front()->origin_y+regionSize < WORLD_SIZE-regionSize))
	{
		list<region*> newRow;
		newRow.push_back(new region(regionSize+1,visibleRegions.front().front()->origin_x,visibleRegions.back().front()->origin_y+regionSize,this));
		visibleRegions.push_back(newRow);
	}
	// REMOVAL OF EXTRA REGIONS
	for (list<list<region*> >::iterator it = visibleRegions.begin();it!=visibleRegions.end();it++)
	{
		if ((*it).size()>0)
			if ((*it).front()->origin_x < (regionXIndex-sizeOfVisibleSquareInRegions/2-1)*regionSize)
			{
				delete (*it).front();
				(*it).pop_front();
			}
		if ((*it).size()>0)
			if ((*it).back()->origin_x > (regionXIndex+sizeOfVisibleSquareInRegions/2+1)*regionSize)
			{
				delete (*it).back();
				(*it).pop_back();
			}
	}
	if (visibleRegions.front().size()==0)
		visibleRegions.pop_front();
	else	
		if (visibleRegions.front().front()->origin_y < (regionYIndex-sizeOfVisibleSquareInRegions/2-1)*regionSize)
		{
			for (list<region*>::iterator it = visibleRegions.front().begin();it!=visibleRegions.front().end();it++)
				delete *it;
			visibleRegions.pop_front();
		}
	if (visibleRegions.back().size()==0)
		visibleRegions.pop_back();
	else
		if (visibleRegions.back().front()->origin_y > (regionYIndex+sizeOfVisibleSquareInRegions/2+1)*regionSize)
		{
			for (list<region*>::iterator it = visibleRegions.back().begin();it!=visibleRegions.back().end();it++)
				delete *it;
			visibleRegions.pop_back();
		}
	// RENDERING OF REGIONS
	int nr = 0;
	for (list<list<region*> >::iterator it = visibleRegions.begin();it!=visibleRegions.end();it++)
		for (list<region*>::iterator it2 = (*it).begin(); it2!=(*it).end() ; it2++)
		{
			nr++;
			float thisDetail = renderDetail - dist((*it2)->origin_x,(*it2)->origin_y,x,y) / (2*regionSize);
			(*it2)->Render(max(3,(int)thisDetail));
		}
	/*
	for (list<list<region*> >::iterator it = visibleRegions.begin();it!=visibleRegions.end();it++)
		for (list<region*>::iterator it2 = (*it).begin(); it2!=(*it).end() ; it2++)
		{
			float thisDetail = renderDetail - dist((*it2)->x,(*it2)->y,x,y) / (2*regionSize);
			(*it2)->RenderGrass(max(3,(int)thisDetail));
		}
	*/
	dataBook->deleteUnused();
		
	// GUI INTERESTING INFO
	writeString(10,60,"Visible %dx%dm (%d regions)",sizeOfVisibleSquareInRegions*regionSize
																								 ,sizeOfVisibleSquareInRegions*regionSize
																								 ,nr);
	writeString(400,20,"Total world size: %dx%dm",worldSize,worldSize);
	writeString(400,40,"%d pages of size %d",dataBook->getNumPages(),PAGE_SIZE);
	writeString(400,60,"TerrainDetail Level: %d",renderDetail);

	writeString(800,20,"Temperature: %.3f (%dC)",getBiomeAt(y,x)->temperature,(int)(getBiomeAt(y,x)->temperature*80)-20);
	writeString(800,40,"Moisture: %.3f",getBiomeAt(y,x)->moisture);
	writeString(800,60,"Grassiness: %.3f",getBiomeAt(y,x)->grassiness);
	writeString(800,80,"Climate number: %d",getBiomeAt(y,x)->climate);
	writeString(1200,20,"Topography Contrast Level: %f",getBiomeAt(y,x)->topography);
	writeString(1200,40,"Detail Contrast Level: %f",getBiomeAt(y,x)->detail);
}

float World::cellHeight(int world_y,int world_x)
{
	//TODO fixme
	//assert(world_y>=0 && world_y<=WORLD_SIZE);
	//assert(world_x>=0 && world_x<=WORLD_SIZE);
	world_y=world_y<0?0:(world_y>WORLD_SIZE?WORLD_SIZE:world_y);
	world_x=world_x<0?0:(world_x>WORLD_SIZE?WORLD_SIZE:world_x);
	return hm->getAt(world_y,world_x);
}
sbit* World::getSAt(int world_y,int world_x)
{
	world_y=world_y<0?0:(world_y>WORLD_SIZE?WORLD_SIZE:world_y);
	world_x=world_x<0?0:(world_x>WORLD_SIZE?WORLD_SIZE:world_x);	
	//assert(y>0 && y<=WORLD_SIZE);
	//assert(x>0 && x<=WORLD_SIZE);
	return dataBook->getAt(world_y,world_x);
}

void World::Reset()
{
	visibleRegions.clear();
}

void World::setRenderDistance(float newRend)
{
	 sizeOfVisibleSquareInRegions = (int)newRend/regionSize;
}

float World::getRenderDistance()
{
	return sizeOfVisibleSquareInRegions*regionSize;
}

int World::getRegionSize()
{
	return regionSize;
}

biomeData* World::getBiomeAt(int y,int x)
{
	assert(y>=0 && y<=WORLD_SIZE);
	assert(x>=0 && x<=WORLD_SIZE);
	return hm->biomes->getAt(y,x);
}