#include "grass.h"
#include "textures.h"
#include "graphics.h"
#include <stdio.h>
#include <GL/glut.h>
#include <assert.h>
GLuint grassTexture = 0;

grass::grass(Vector3 _position,Vector3 _normal,World* _parent,float _height)
{
	parent = _parent;
	position = _position;
	height = _height;
	normal = _normal;
	if (grassTexture==0)
	{
		unsigned char * data = new unsigned char[256 * 256 * 4];
		FILE * file = fopen("grassrgba256.raw", "rb" );
		assert(file !=NULL);
		fread( data, 256 * 256 * 4, 1, file );
		fclose( file );
		grassTexture = LoadAlphaTextureFromData(data,256,256);
		delete [] data;
	}
}

void grass::Render()
{
	Vector3* col = new Vector3(parent->getSAt((int)position.z,(int)position.x)->colour);
	glColor3f((GLfloat)(col->x/256.0),(GLfloat)(col->y/256.0),(GLfloat)(col->z/256.0));
	glBindTexture( GL_TEXTURE_2D, grassTexture );
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.0,1.0);
	glVertex3f(position.x-1,position.y+0,position.z+0);
	glTexCoord2d(1.0,1.0);
	glVertex3f(position.x+1,position.y+0,position.z+0);
	glTexCoord2d(1.0,0.0);
	glVertex3f(position.x+1,position.y+height,position.z+0);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.0,0.0);
	glVertex3f(position.x-1,position.y+height,position.z+0);
	glTexCoord2d(0.0,1.0);
	glVertex3f(position.x-1,position.y+0,position.z+0);
	glTexCoord2d(1.0,0.0);
	glVertex3f(position.x+1,position.y+height,position.z+0);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.0,1.0);
	glVertex3f(position.x+0,position.y+0,position.z-1);
	glTexCoord2d(1.0,1.0);
	glVertex3f(position.x+0,position.y+0,position.z+1);
	glTexCoord2d(1.0,0.0);
	glVertex3f(position.x+0,position.y+height,position.z+1);
	glEnd();
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0.0,1.0);
	glVertex3f(position.x+0,position.y+0,position.z-1);
	glTexCoord2d(0.0,0.0);
	glVertex3f(position.x+0,position.y+height,position.z-1);
	glTexCoord2d(1.0,0.0);
	glVertex3f(position.x+0,position.y+height,position.z+1);
	glEnd();
	addTriangles(4);
	delete col;
}
