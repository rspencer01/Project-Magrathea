#include "book.h"
#include <assert.h>
#include <stdio.h>

book::book(World* _p)
{
	parent = _p;
	for(int y = 0;y<PAGE_COUNT;y++)
		for(int x = 0;x<PAGE_COUNT;x++)
			pages[y][x] = NULL;
	numInitPages = 0;
}

page* book::forcePage(int pageY,int pageX,int state)
{
	if (pageY<0 || pageY> PAGE_COUNT-1 || pageX<0 || pageX>PAGE_COUNT-1)
		return NULL;
	if (pages[pageY][pageX]==NULL)
	{
		pages[pageY][pageX] = new page(pageX,pageY,this,parent);
		numInitPages++;
	}
	pages[pageY][pageX]->updateTo(state);
	return pages[pageY][pageX];
}

sbit* book::getPartialAt(int y,int x)
{
	int pageX = x/PAGE_SIZE;
	int pageY = y/PAGE_SIZE;	
	if (pages[pageY][pageX]==NULL)
		return NULL;
	return pages[pageY][pageX]->getAt(y%PAGE_SIZE,x%PAGE_SIZE);
}

sbit* book::getAt(int y,int x)
{
	assert(x>=0 && x<WORLD_SIZE);
	assert(y>=0 && y<WORLD_SIZE);
	int pageX = x/PAGE_SIZE;
	int pageY = y/PAGE_SIZE;
	forcePage(pageY,pageX,PS_ELEVATIONS|PS_COLOUR|PS_NORMALS|PS_GRASS|PS_SURFACE);
	sbit* reply = pages[pageY][pageX]->getAt(y%PAGE_SIZE,x%PAGE_SIZE);
	return reply;
}

int book::getNumPages()
{
	return numInitPages;
}

void book::deleteUnused()
{
	for(int y = 0;y<PAGE_COUNT;y++)
		for(int x = 0;x<PAGE_COUNT;x++)
			if (pages[y][x]!=NULL)
				if (pages[y][x]->toBeDeleted())
				{
					delete pages[y][x];
					pages[y][x] = NULL;
					numInitPages--;
				}
}