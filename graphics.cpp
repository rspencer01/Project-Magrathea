#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "graphics.h"



Vector3 getNormal(float x1,float y1,float z1,float x2,float y2,float z2)
{
	Vector3 ans;
	ans.x = y1*z2-z1*y2;
	ans.y = z1*x2-x1*z2;
	ans.z = x1*y2-y1*x2;
	ans.normalise();
	return ans;
}

Vector3 getNormal3f(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3)
{
	return getNormal(x1-x2,y1-y2,z1-z2,x3-x2,y3-y2,z3-z2);
}

int renderedTriangles;
int frameCount = 0;
int currentTime;
int previousTime;
float fps;

void calculateFPS()
{
  frameCount++;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  int timeInterval = currentTime - previousTime;
  if(timeInterval > 1000)
  {
    fps = frameCount / (timeInterval / 1000.0f);
    previousTime = currentTime;
    frameCount = 0;
  }
}

void writeString(int x, int y,const char* format, ... )
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.0, 0.0, 0.0); // Green
  
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  
  glRasterPos2i(x,glutGet(GLUT_WINDOW_HEIGHT)-y);
  
  for (p = buffer; *p; p++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();  
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}

void resetStats()
{
	renderedTriangles = 0;
}
void addTriangles(int x)
{
	renderedTriangles += x;
}


void drawFPS()
{
	calculateFPS();
	writeString(10,20,"FPS: %.2f",fps);
	writeString(10,40,"Triangles: %d",renderedTriangles);
}


bool fullDisplay = false;

void toggleHUD()
{
	fullDisplay = ! fullDisplay;
}

void displayHUD()
{
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	if (fullDisplay)
	{
		writeString(10,windowHeight-100,"Display/Hide HUD: <f>");
		writeString(10,windowHeight-80,"Move: <w><s><a><d>");
		writeString(10,windowHeight-60,"Look Up/Down <q><e>");
		writeString(10,windowHeight-40,"Display Wireframe <space>");
		writeString(10,windowHeight-20,"Toggle POV <o>");
		writeString(300,windowHeight-100,"Reset World: <r>");
		writeString(300,windowHeight-80,"Increase Decrease Render Distance: <+><->");
		writeString(300,windowHeight-60,"Increase Decrease Render Detail: <t><g>");
		writeString(300,windowHeight-40,"Goto position: <h>");
		writeString(300,windowHeight-20,"Increase Decrease Spped: <x><z>");
	}
	else
	{
		writeString(10,windowHeight-20,"<f>");
	}
}