#include <GL/glut.h>
#include <math.h>
#include "graphics.h"

void RenderSky(float x, float y)
{
	glBindTexture(GL_TEXTURE_2D,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.08984f,0.39453f,0.80859f);
	glVertex3f(x,600.f,y);
	glColor3f(163/256.0,241/256.0,255/256.0);
	for (int i = 0;i<=20;i++)
		glVertex3f(x+1000*sin(i/20.f*3.1415*2),0.f,y+1000*cos(i/20.f*3.1415*2));
	glEnd();
	addTriangles(20);
}