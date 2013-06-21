#ifndef BOOK_H
#define BOOK_H

class book;

#include "magnathea.h"
#include "page.h"

#define PAGE_COUNT (WORLD_SIZE/PAGE_SIZE + 1)

class book
{
	private:
		page* pages [PAGE_COUNT][PAGE_COUNT];
		World* parent;
		int numInitPages;
	public:
		book(World*);
		sbit* getPartialAt(int y,int x);
		sbit* getAt(int,int);
		page* forcePage(int,int,int);
		int getNumPages();
		void deleteUnused();
};


#endif 