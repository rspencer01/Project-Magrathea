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
	if (! (state&PS_INITIALISED))
		return;
}

void page::updateTo(int value)
{
	lastTouched = (int)time(NULL);
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
	if (value&PS_SURFACE&(~state))
		DoSurface();
	if (value&PS_GRASS&(~state))
		DoGrass();
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
			data[y][x].normal[0] = data[y][x].normal[2] = 0; data[y][x].normal[1] = 1;
			data[y][x].colour[0] = data[y][x].colour[1] = data[y][x].colour[2] = 1;
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
			data[y][x].colour[0] = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[0];
			data[y][x].colour[1] = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[1];
			data[y][x].colour[2] = parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->grassColor[2];
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
					if (data[y][x].surfaceType == SURFACE_GRASS)
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
			data[y][x].surfaceType = SURFACE_GRASS;
			//Todo.  Split into cases
			if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->baseLevel > 0)
			{
				if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate == CL_COASTAL)
					data[y][x].surfaceType = SURFACE_SAND;
			}
			else
				if (data[y][x].elevation < 10)
					data[y][x].surfaceType = SURFACE_SAND;	
			
			float rockCutoff = 0.92;
			float dirtCutoff = 0.95;
			if (parentW->getBiomeAt(y+origY*PAGE_SIZE,x+origX*PAGE_SIZE)->climate == CL_TUNDRA)
				rockCutoff = 0.95;
			//if (data[y][x].normal->y < dirtCutoff)
			//	data[y][x].surfaceType = SURFACE_DIRT;
			for (int dy = max(y-1,0);dy<min(y+2,PAGE_SIZE);dy++)
				for (int dx = max(x-1,0);dx<min(x+2,PAGE_SIZE);dx++)
					if (data[dy][dx].normal[1] < rockCutoff)
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
			
			data[y - origY*PAGE_SIZE][x - origX*PAGE_SIZE].normal[0] = n.x;
			data[y - origY*PAGE_SIZE][x - origX*PAGE_SIZE].normal[1] = n.y;
			data[y - origY*PAGE_SIZE][x - origX*PAGE_SIZE].normal[2] = n.z;
		}
	
}

bool page::toBeDeleted()
{
	return time(NULL)-lastTouched> PAGE_EXPIRE_TIME;
}