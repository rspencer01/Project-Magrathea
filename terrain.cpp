#include "textures.h"
#include "terrain.h"
#include "misc.h"
#include "biome.h"
#include <assert.h>
#include <stdio.h>
#include <GL/glut.h>
using namespace std;
const char*    textureNames[TERRAIN_COUNT] = {"swater512.raw","srock512.raw","ssand512.raw","sdirt512.raw","ssnow512.raw","solidGrass512.raw","sgrassb512.raw"};
unsigned char* textureData[TERRAIN_COUNT];

void initTerrain()
{
	for (int i = 0;i<TERRAIN_COUNT;i++)
		textureData[i] = loadRaw(textureNames[i],TERRAIN_RAW_SIZE);
}

inline float linearInter(float x1,float x2,float t)
{
	return x1*t+x2*(1.0-t);
}

int repIndex(int a,int b,int c)
{
	if (c%2==0)
	{
		int d = a;
		a = b;
		b = d;
	}
	return ((a+TERRAIN_RAW_SIZE)%TERRAIN_RAW_SIZE)*TERRAIN_RAW_SIZE + ((b+TERRAIN_RAW_SIZE)%TERRAIN_RAW_SIZE);
}


void RenderCanvasBegin (int left, int right, int bottom, int top, int size)
{

  glDisable (GL_CULL_FACE);
  glDisable (GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_TEXTURE_2D);
  glViewport (0, 0, size, size);
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  glOrtho (left, right, bottom, top, 0.1f, 2048);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity();
  glTranslatef(0, 0, -10.0f);

}

void RenderCanvasEnd ()
{
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();  
  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	
  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();  
}


GLuint MakeCompositeTerrain(int size,World* parent,int detail,int X,int Y)
{
	int textureSize = 100;
	GLuint textureId;
	
	//New texture
	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
 	glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//Fill it with nothing
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//Save viewport
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,(int*)viewport);
	
	RenderCanvasBegin(0,0,textureSize,textureSize,textureSize);


	glClearColor (1,0,0,1);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	//Write the output to the texture
	glBindTexture(GL_TEXTURE_2D, textureId);
  //And copy it into the big texture
	glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);
	RenderCanvasEnd();
	//Restore viewport
	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
	
	return textureId;
}
