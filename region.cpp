#include <GL/glew.h>
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
	origin_x = _x;
	origin_y = _y;
	parent = _parent;
	TriangleData = NULL;
	VertexData = NULL;
	TextureData = NULL;
	surface =NULL;
	numTri = NULL;
	bestDetail = -1;
	finishedTexture = false;
	TextureNumber = -1;
	texX = texY = 0;
	vertexVBO = -1;
	textureVBO = -1;
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
		glGenBuffersARB(1,&vertexVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, size*size*3*sizeof(float), VertexData, GL_STATIC_DRAW_ARB);

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
		glGenBuffersARB(1,&textureVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, textureVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, size*size*2*sizeof(float), TextureData, GL_STATIC_DRAW_ARB);
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

void region::doPatch(int patchx,int patchy)
{
  int texture_step = size / patch_steps;
//Three heightmap points below where we are
  int startx = texX * texture_step - 3;
  int starty = texY * texture_step - 3;
  int endx = startx + texture_step + 5;
  int endy = starty + texture_step + 6;
  glColor3f(1,0,0);
  glBindTexture(GL_TEXTURE_2D,getTerrainTexture(SURFACE_ROCK));
  for (int y = starty; y < endy - 1; y++)
  {
    glBegin (GL_QUAD_STRIP);
    for (int x = startx; x < endx; x++) 
	{
      int world_x = origin_x + x;
      int world_y = origin_y + y;
      glTexCoord2f ((float)x / 8, (float)y / 8);
	  float* surface_color = parent->getSAt(world_x, world_y)->colour;
      glColor3f (surface_color[0]/256,surface_color[1]/256,surface_color[2]/256);
      glVertex2f ((float)x, (float)y);
      glTexCoord2f ((float)x / 8, (float)(y + 1) / 8);
      surface_color = parent->getSAt(world_x, world_y+1)->colour;
      glColor3f (surface_color[0]/256,surface_color[1]/256,surface_color[2]/256);
      glVertex2f ((float)x, (float)(y + 1));
    }
    glEnd ();
  }

  for (int stage = 0; stage < TERRAIN_COUNT; stage++) 
  {
    glBindTexture (GL_TEXTURE_2D, getTerrainTexture(stage));
	glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	

    for (int y = starty; y < endy - 1; y++) 
	{
      for (int  x = startx; x < endx; x++) 
	  {
        int world_x = origin_x + x;
        int world_y = origin_y + y;
		int surfaceNumber = parent->getSAt(world_y, world_x)->surfaceType;
        if (surfaceNumber != stage)
          continue;
        float posx = (float)x;
        float posy = (float)y;
        float tile = 0.66f*1.5; 
        glPushMatrix ();
        glTranslatef (posx - 0.5f, posy - 0.5f, 0);
		//Do some funky rotation, please
        int angle = (world_x + world_y * 1000007) * 25;
        angle %= 360;
        glRotatef ((float)angle, 0.0f, 0.0f, 1.0f);
		//Move us to here please
        glTranslatef (-posx, -posy, 0);
		float lightingMultiplyer = Vector3(parent->getSAt(world_y, world_x)->normal).dot(Vector3(1,1,0));
		if (surfaceNumber == SURFACE_GRASS)
		{
			float* surface_color = parent->getSAt(world_y, world_x)->colour;
			glColor3f (surface_color[0]/256*lightingMultiplyer,surface_color[1]/256*lightingMultiplyer,surface_color[2]/256*lightingMultiplyer);
		}
		else
			glColor3f (lightingMultiplyer,lightingMultiplyer,lightingMultiplyer);
        glBegin (GL_QUADS);
        glTexCoord2f (0,0); glVertex2f (posx - tile, posy - tile);
        glTexCoord2f (1,0); glVertex2f (posx + tile, posy - tile);
        glTexCoord2f (1,1); glVertex2f (posx + tile, posy + tile);
        glTexCoord2f (0,1); glVertex2f (posx - tile, posy + tile);
        glEnd ();
        glPopMatrix ();
      }
    }
  }




}


void region::doNextTexture()
{
#define TEXTURE_SIZE 2048
  if (finishedTexture)
	return;

  if ((texX==texY+1 && texY == patch_steps-1))
  {
	  finishedTexture = true;
	  return;
  }
  
  
#define TERRAIN_PATCH (size/_patch_size)
  int viewport [4];
  glGetIntegerv(GL_VIEWPORT,viewport);

  if (TextureNumber == -1)
  {
	glGenTextures (1, &TextureNumber); 
		//Set it up
	glBindTexture(GL_TEXTURE_2D, TextureNumber);

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //We draw the terrain texture in squares called patches, but how big should they be?
    //We can't draw more than will fit in the viewport
    _patch_size = min (256, TEXTURE_SIZE);
    patch_steps = TEXTURE_SIZE / _patch_size;
    //We also don't want to do much at once. Walking a 128x128 grid in a singe frame creates stuttering. 
    while (size / patch_steps > 64) 
	{
      _patch_size /= 2;
      patch_steps = TEXTURE_SIZE / _patch_size;
    }	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	_patch_size;
  }
  //We do a little bit at a time...
  if (texX == patch_steps)
  {
	  texY++;
	  texX = 0;
  }
  {
	RenderCanvasBegin (texX * (size / patch_steps), texX * (size / patch_steps) + (size / patch_steps), texY * (size / patch_steps), texY * (size / patch_steps) + (size / patch_steps), _patch_size);
	doPatch (texX, texY);
	glBindTexture(GL_TEXTURE_2D, TextureNumber);
	//And copy it into the big texture
	glCopyTexSubImage2D (GL_TEXTURE_2D, 0, texX * _patch_size, texY * _patch_size, 0, 0, _patch_size, _patch_size);
	RenderCanvasEnd ();
  }
  texX++;
  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
}

void region::Render(int detail)
{
	if (surface==NULL)
		populate();
	if (!finishedTexture)
	{
		doNextTexture();
	}
	Triangulate(detail);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor3f(1,1,1);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertexVBO);
	glVertexPointer( 3,   //3 components per vertex (x,y,z)
                 GL_FLOAT,
                 0,
                 0);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,textureVBO);
    glTexCoordPointer(2,
  					  GL_FLOAT,
  					  0,
  					  0);
	glBindTexture(GL_TEXTURE_2D, TextureNumber);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
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
				surface[getIndex(i,j)] = parent->getSAt(origin_y+i,origin_x+j);
				if (surface[getIndex(i,j)]->isGrass)
				{
					grasses.push_back(grass(Vector3(surface[getIndex(i,j)]->x,surface[getIndex(i,j)]->elevation,surface[getIndex(i,j)]->y),Vector3(surface[getIndex(i,j)]->normal),
																	parent,1.5f*surface[getIndex(i,j)]->grassHeight));
					grasses.back().Render();
				}
			}
	}
}