#include <GL/glew.h>
#include <GL/gl.h>
#include "region.h"
#include "graphics.h"
#include "terrain.h"
#include "misc.h"
#include "textures.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>


#define MAX_TEXTURE_SIZE 2048

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
	numTri = NULL;
	bestDetail = -1;
	lastTexture = -1;
	TextureNumber = -1;
	texX = texY = 0;
	dataVBO = -1;
	initialised = false;
	gotGrass = false;
}

region::~region()
{
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
	if (dataVBO==-1)
	{
		float* VertexAndTextureData = new GLfloat[size*size*5];
		for (int y = 0;y<size;y++)
			for (int x = 0;x<size;x++)
			{
				sbit* here = parent->getSAt(origin_y+y,origin_x+x);
				VertexAndTextureData[(y*size+x)*3 + 0] = here->x;
				VertexAndTextureData[(y*size+x)*3 + 1] = here->elevation>0?here->elevation:0;
				VertexAndTextureData[(y*size+x)*3 + 2] = here->y;
			}
		for (int i = 0;i<size;i++)
			for (int j = 0;j<size;j++)
			{
				VertexAndTextureData[size*size*3+(i*size+j)*2 + 0] = (GLfloat)((j)/float(size));
				VertexAndTextureData[size*size*3+(i*size+j)*2 + 1] = (GLfloat)((i)/float(size));
			}
		glewInit();
		glGenBuffersARB(1,&dataVBO);
		glBindBufferARB(GL_ARRAY_BUFFER, dataVBO);
		glBufferDataARB(GL_ARRAY_BUFFER, size*size*5*sizeof(float), VertexAndTextureData,GL_STATIC_DRAW);
		delete[] VertexAndTextureData;
		
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
				TriangleData[detail][count++] = ((i-adetail)*size)+j-adetail;
				TriangleData[detail][count++] = (i*size)+j-adetail;
				TriangleData[detail][count++] = (i*size)+j;
				TriangleData[detail][count++] = ((i-adetail)*size)+j;
				TriangleData[detail][count++] = ((i-adetail)*size)+j-adetail;
			}		
		numTri[detail] = count;
		glGenBuffersARB(1,&indexVBO);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(int), TriangleData[detail], GL_STATIC_DRAW);
	}	
}

void region::doPatch()
{
//Three heightmap points below where we are
  int startx =  - 3;
  int starty =  - 3;
  int endx =  size + 3;
  int endy = size + 3;
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
      glColor3f (1,1,1);
      glVertex2f ((float)x, (float)y);
      glTexCoord2f ((float)x / 8, (float)(y + 1) / 8);
      surface_color = parent->getSAt(world_x, world_y+1)->colour;
      glColor3f (1,1,1);
      glVertex2f ((float)x, (float)(y + 1));
    }
    glEnd ();
  }

  for (int stage = 0; stage < TERRAIN_COUNT; stage++) 
  {
	if (stage == SURFACE_ROCK)
		continue;
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
        glTranslatef (posx + 1.0f, posy + 1.0f, 0);
		//Do some funky rotation, please
        int angle = (world_x + world_y * 1000007) * 25;
        angle %= 360;
        glRotatef ((float)angle, 0.0f, 0.0f, 1.0f);
		//Move us to here please
        glTranslatef (-posx, -posy, 0);
		if (surfaceNumber == SURFACE_GRASS)
		{
			float* surface_color = parent->getSAt(world_y, world_x)->colour;
			glColor3f (surface_color[0]/256,surface_color[1]/256,surface_color[2]/256);
		}
		else
			glColor3f (1,1,1);
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

  GLuint fboId = -1;
  GLuint rboId = -1;

void region::doNextTexture(int detail)
{
  if (lastTexture == detail)
	return;
  printf("New texture of detail %d\n",detail);
  if (TextureNumber!=-1)
	  glDeleteTextures(1,&TextureNumber);
  int textureSize = min(MAX_TEXTURE_SIZE,1<<detail);
  glGenTextures(1, &TextureNumber);
  glBindTexture(GL_TEXTURE_2D, TextureNumber);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  int viewport [4];
  glGetIntegerv(GL_VIEWPORT,viewport);


  //Create renderbuffer
  if (rboId==-1)
	glGenRenderbuffers(1, &rboId);
  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                      textureSize, textureSize);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //Create framebuffer
  if (fboId==-1)
	glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                       GL_TEXTURE_2D, TextureNumber, 0);

  // attach the renderbuffer to depth attachment point
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                          GL_RENDERBUFFER, rboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // draw...
  RenderCanvasBegin (0,size,0,size,textureSize);
  glClearColor (skyColor[0],skyColor[1],skyColor[2],skyColor[3]);
	
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glColor3f(0,0,1);
  doPatch();
  RenderCanvasEnd();

  //Cleanup
  const GLenum discards[]  = {GL_COLOR_ATTACHMENT0};
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D,TextureNumber);

  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
  lastTexture = detail;
}

void region::Render(int detail)
{
	if (detail>12)
		detail = 12;
	doNextTexture(detail);
	Triangulate(detail);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER,dataVBO);
	glVertexPointer( 3,   //3 components per vertex (x,y,z)
                 GL_FLOAT,
                 0,
                 0);
    glTexCoordPointer(2,
  					  GL_FLOAT,
  					  0,
  					  (void*)(size*size*3*sizeof(float)));
	glBindTexture(GL_TEXTURE_2D, TextureNumber);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,indexVBO);
	glDrawElements( GL_TRIANGLES, //mode
                  numTri[detail],  //count, ie. how many indices
                  GL_UNSIGNED_INT, //type of the index array
                  0);
  
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindBufferARB(GL_ARRAY_BUFFER,0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	addTriangles(numTri[detail]/3);
}

void region::RenderGrass(int detail)
{
	if (!gotGrass)
	{
		for (int y = 0;y<size;y++)
			for (int x = 0;x<size;x++)
				if (parent->getSAt(origin_y+y,origin_x+x)->isGrass)
				{
					grasses.push_back(grass(Vector3(origin_x+x,parent->getSAt(origin_y+y,origin_x+x)->elevation,origin_y+y),Vector3(parent->getSAt(origin_y+y,origin_x+x)->normal),parent,parent->getSAt(origin_y+y,origin_x+x)->grassHeight));
				}
	}
	gotGrass = true;
	if (detail > 7)
		for (unsigned int i = 0;i<grasses.size();i++)
			grasses[i].Render();		
}
