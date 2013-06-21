#ifndef PAGE_H
#define PAGE_H

#include "magrathea.h"

//This must be carfully chosen to divide WORLDSIZE/2
#define PAGE_SIZE        256
#define PAGE_EXPIRE_TIME 1000

#define PS_UNINITIALISED   0x00
#define PS_INITIALISED     0x01
#define PS_ELEVATIONS      0x03
#define PS_COLOUR          0x05
#define PS_NORMALS         0x0B
#define PS_GRASS           0x13
#define PS_SURFACE         0x2B


class page;

#include "world.h"

class page
{
	private:
		int state;
		sbit data[PAGE_SIZE][PAGE_SIZE];
		int origX;
		int origY;
		World* parentW;
		book* parentB;
		void DoElevation();
		void DoColour();
		void DoNormals();
		void DoGrass();
		void DoInitialisation();
		void DoSurface();
		int lastTouched;
	public:
		page(int,int,book*,World*);
		~page();
		void updateTo(int);
		sbit* getAt(int,int);
		bool toBeDeleted();
		
};

#endif