#include "region.h"
#include "graphics.h"
#include "terrain.h"
#include "misc.h"
#include "textures.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
//
// Contains the code for the render regions: blocks of terrain that have one texture draped over them.
//
//

region::region(int _size,int _x,int _y,World* _parent)
{
	size = _size;
	x = _x;
	y = _y;
	parent = _parent;
	TriangleData = NULL;
	VertexData = NULL;
	TextureData = NULL;
	surface =NULL;
	numTri = NULL;
	bestDetail = -1;
}

region::~region()
{
	if (surface!=NULL)
		delete[] surface;
	if (VertexData!=NULL)
		delete[] VertexData;
	if (TextureData!=NULL)
		delete[] TextureData;
	if (TriangleData!=NULL)
	{
		for (int i = 0;i<20;i++)
			delete[] TriangleData[i];
		delete[] TriangleData;
	}
	if (numTri!=NULL)
		delete[] numTri;
	glDeleteTextures( 1, &TextureNumber);
}

int region::getIndex(int x,int y)
{
	return x+y*size;
}

void region::Triangulate(int detail)
{
	if (VertexData==NULL)
	{
		VertexData = new GLfloat[size*size*3];
		for (int i = 0;i<size*size;i++)
		{
			VertexData[i*3 + 0] = (GLfloat)surface[i]->x;
			VertexData[i*3 + 1] = (GLfloat)surface[i]->elevation>0?(GLfloat)surface[i]->elevation:0;
			VertexData[i*3 + 2] = (GLfloat)surface[i]->y;
		}
	}
	if (TextureData==NULL)
	{
		TextureData = new GLfloat[size*size*2];	
		for (int i = 0;i<size;i++)
			for (int j = 0;j<size;j++)
			{
				TextureData[(i*size+j)*2 + 0] = (GLfloat)((i)/float(size));
				TextureData[(i*size+j)*2 + 1] = (GLfloat)((j)/float(size));
			}
	}
	/*This is the meat of this function*/
	if (TriangleData==NULL)
	{
		TriangleData = new int*[20];
		numTri = new int [20];
		for (int i = 0;i<20;i++)
			TriangleData[i] = NULL;
	}
	int adetail = 1<<( 8-max(0,min(8,detail)));
	if (TriangleData[detail]==NULL)
	{
		TriangleData[detail]= new int[size*size*6];
		int count = 0;
		for (int i = adetail;i<size;i+=adetail)
			for (int j = adetail;j<size;j+=adetail)
			{
				TriangleData[detail][count++] = (i*size)+j;
				TriangleData[detail][count++] = (i*size)+j-adetail;
				TriangleData[detail][count++] = ((i-adetail)*size)+j-adetail;
				TriangleData[detail][count++] = (i*size)+j;
				TriangleData[detail][count++] = ((i-adetail)*size)+j;
				TriangleData[detail][count++] = ((i-adetail)*size)+j-adetail;
			}		
		numTri[detail] = count;
	}	
}


void region::Render(int detail)
{
	if (surface==NULL)
		populate();
	if (bestDetail<detail)
	{
		if (bestDetail>0)
			glDeleteTextures( 1, &TextureNumber);
		TextureNumber = MakeCompositeTerrain(size,parent,detail,x,y);		
		bestDetail = detail;
	}
	Triangulate(detail);
	


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor3f(1,1,1);
	glVertexPointer( 3,   //3 components per vertex (x,y,z)
                 GL_FLOAT,
                 0,
                 VertexData);	
    glTexCoordPointer(2,
  									GL_FLOAT,
  									0,
  									TextureData);
				 
	glBindTexture(GL_TEXTURE_2D, TextureNumber);
	glDrawElements( GL_TRIANGLES, //mode
                  numTri[detail],  //count, ie. how many indices
                  GL_UNSIGNED_INT, //type of the index array
                  TriangleData[detail]);
  
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	addTriangles(numTri[detail]/3);
	

	
}

void region::RenderGrass(int detail)
{
	if (detail > 7)
		for (unsigned int i = 0;i<grasses.size();i++)
			grasses[i].Render();		
}

void region::populate()
{
	if (surface == NULL)
	{
		surface = new sbit*[size*size];
		for (int i = 0;i<size;i++)
			for (int j = 0;j<size;j++)
			{
				surface[getIndex(i,j)] = parent->getSAt(y+i,x+j);
				if (surface[getIndex(i,j)]->isGrass)
				{
					grasses.push_back(grass(Vector3(surface[getIndex(i,j)]->x,surface[getIndex(i,j)]->elevation,surface[getIndex(i,j)]->y),
																	parent,1.5*surface[getIndex(i,j)]->grassHeight));
					grasses.back().Render();
				}
			}
	}
}