#include <GL/glut.h>
#include <math.h>
#include "magrathea.h"
#include "graphics.h"

void RenderSky(float x, float z)
{
	// No textures please
	glBindTexture(GL_TEXTURE_2D,0);
	// We will make a cone of triangles
	glBegin(GL_TRIANGLE_FAN);
	// A dark blue for the high sky.  The light colour is defined in magrathea.h
	glColor3f(0.08984f,0.39453f,0.80859f);
	glVertex3f(x,400.f,z);
	glColor3fv(skyColor);
	// The sin and cos describes a circe surrounding the given position.
	for (int i = 0;i<=21;i++)
		glVertex3f(x+1000*sin(i/20.f*3.1415*2),0.f,z+1000*cos(i/20.f*3.1415*2));
	glEnd();
	// And just for the hell of it, tell me how many triangles this contributes to the total.
	addTriangles(21);
}
