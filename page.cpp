#include "page.h"
#include "graphics.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

page::page(int _origX, int _origY,book* _bparent,World* _parent)
{
	parentW = _parent;
	parentB = _bparent;
	origX = _origX;
	origY = _origY;
	state = PS_UNINITIALISED;
	updateTo(PS_INITIALISED);
	lastTouched = (int)time(NULL);
}
page::~page()
{
	for(int y = 0;y<PAGE_SIZE;y++)
		for(int x = 0;x<PAGE_SIZE;x++)
		{
			delete data[y][x].normal;
			delete data[y][x].colour;
		}
}

void page::updateTo(int value)
{
	if ((state&value)==value)
		return;
	if (value&PS_INITIALISED&(~state))
		DoInitialisation();
	if (value&PS_ELEVATIONS&(~state))
		DoElevation();
	if (value&PS_COLOUR&(~state))
		DoColour();
	if (value&PS_NORMALS&(~state))
		DoNormals();
	if (value&PS_GRASS&(~state))
		DoGrass();
	if (value&PS_SURFACE&(~state))
		DoSurface();
	lastTouched = (int)time(NULL);	
}

void page::DoInitialisation()
{
	for(int y = 0;y<PAGE_SIZE;y++)
		for(int x = 0;x<PAGE_SIZE;x++)
		{
			data[y][x].x =         origX*PAGE_SIZE + x;
			data[y][x].y =         origY*PAGE_SIZE + y;
			data[y][x].elevation = 0;
			data[y][x].waterLevel = 0;
			data[y][x].normal = new Vector3(0,1,0);
			data[y][x].colour = new Vector3(rand()%100,rand()%100,rand()%100);
			data[y][x].isGrass = false;
			data[y][x].surfaceType = SURFACE_GRASS;
		}
	state = state | PS_INITIALISED;
}

void page::DoColour()
{
	for(int y = 0;y<PAGE_SIZE;y++)
		for(int x = 0;x<PAGE_SIZE;x++)
		{
			data[y][x].colour->x = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[0];
			data[y][x].colour->y = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[1];
			data[y][x].colour->z = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[2];
		}
	state = state | PS_COLOUR;
}


void page::DoElevation()
{
	for(int y = 0;y<PAGE_SIZE;y++)
		for(int x = 0;x<PAGE_SIZE;x++)
			data[y][x].elevation = parentW->cellHeight(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE);
	state = state | PS_ELEVATIONS;
}

void page::DoGrass()
{
	for (int y = 0;y<PAGE_SIZE;y++)
		for (int x = 0;x<PAGE_SIZE;x++)
		{
			if (data[y][x].elevation > data[y][x].waterLevel)
				if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate != CL_COASTAL)
					if (rand()%100 <= parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassiness*100.0)
					{
						data[y][x].isGrass = true;
						data[y][x].grassHeight = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassHeight;
					}
		}
	state = state | PS_GRASS;
}

void page::DoSurface()
{
	for (int y = 0;y<PAGE_SIZE;y++)
		for (int x = 0;x<PAGE_SIZE;x++)
		{
			//Todo.  Split into cases
			if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->baseLevel > 0)
			{
				if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate == CL_COASTAL)
					data[y][x].surfaceType = SURFACE_SAND;
				if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate == CL_TUNDRA)
					data[y][x].surfaceType = SURFACE_SNOW;
			}
			else
				data[y][x].surfaceType = SURFACE_SAND;	
			float rockCutoff = 0.92;
			float dirtCutoff = 0.95;
			if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate == CL_TUNDRA)
				rockCutoff = 0.95;
			if (data[y][x].normal->y < dirtCutoff)
				data[y][x].surfaceType = SURFACE_DIRT;
			if (data[y][x].normal->y < rockCutoff)
				data[y][x].surfaceType = SURFACE_ROCK;
			if (data[y][x].elevation <data[y][x].waterLevel)
				data[y][x].surfaceType = SURFACE_WATER; 
		}
	state = state | PS_SURFACE;
}

sbit* page::getAt(int y,int x)
{
	assert(x>=0 && x<PAGE_SIZE);
	assert(y>=0 && y<PAGE_SIZE);
	assert(state & PS_INITIALISED);
	lastTouched = (int)time(NULL);
	return &(data[y][x]);
}

void page::DoNormals()
{
	state = state | PS_NORMALS;
	for (int dy = max(0,origY-1);dy<=min(PAGE_COUNT-1,origY+1);dy++)
		for (int dx = max(0,origX-1);dx<=min(PAGE_COUNT-1,origX+1);dx++)
			parentB->forcePage(dy,dx,PS_ELEVATIONS);
	for (int y = origY*PAGE_SIZE;y<min(origY*PAGE_SIZE+PAGE_SIZE-1,WORLD_SIZE-2);y++)
		for (int x = origX*PAGE_SIZE;x<min(origX*PAGE_SIZE+PAGE_SIZE-1,WORLD_SIZE-2);x++)
		{
			Vector3 n = getNormal3f(x,parentB->getPartialAt(y+1,x)->elevation,y+1,
									x,parentB->getPartialAt(y,x)->elevation,y,
									x+1,parentB->getPartialAt(y+1,x+1)->elevation,y+1);
			delete data[y - origY*PAGE_SIZE][x - origX*PAGE_SIZE].normal;
			data[y - origY*PAGE_SIZE][x - origX*PAGE_SIZE].normal = new Vector3(n.x,n.y,n.z);
		}
	
}

bool page::toBeDeleted()
{
	return time(NULL)-lastTouched> PAGE_EXPIRE_TIME;
}