#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "magrathea.h"
#include "graphics.h"
#include "camera.h"
#include "world.h"
#include "misc.h"

void display(void);
void reshape(int,int);
void keyPressed (unsigned char,int,int);
void keyUp(unsigned char,int,int);
void keyOperations(void);

bool* keyStates = new bool[256];

World* Island;
CCamera* camera;
float DownlookDist = 0;
bool fp = false;
int speed = 1;

int main(int argc, char**argv)
{
	srand(0);
	
	glutInit(&argc,argv);
	//Lighting is handled by itself.
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Terrain");
	glEnable(GL_DEPTH_TEST);
	glEnable ( GL_COLOR_MATERIAL ) ;
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    glAlphaFunc ( GL_GREATER, (GLclampf)0.1 ) ;
    glEnable ( GL_ALPHA_TEST ) ;
	
	glShadeModel(GL_SMOOTH);

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);

	initTerrain();

	camera = new CCamera();
	camera->SetX(WORLD_SIZE/2.0);
	camera->SetZ(WORLD_SIZE/2.0);
	//camera->SetY(800);
	//camera->RotateY(-90);
	//camera->RotateX(-90);
	
	Island = new World(WORLD_SIZE);


		
	
	glutMainLoop();
}

void display(void)
{
	keyOperations();
	glClearColor(0.1f,1.0f,1.0f,1.f);
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	resetStats();
	
	glMatrixMode(GL_MODELVIEW);
	camera->Render();
	Island->Render(camera->GetX(),camera->GetZ());
	drawFPS();
	writeString(10, 80,"Camera Position: %.2f %.2f %.2f",camera->GetX(),camera->GetY(),camera->GetZ());
	displayHUD();
	glutSwapBuffers();
}

void reshape(int width,int height)
{
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//FOV, Aspect Ratio, Near pane and far plane
	gluPerspective(60,(GLfloat)width/(GLfloat)height,1.0,1500.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyPressed(unsigned char key,int x,int y)
{
	keyStates[key] = true;
	if (key=='o')	
	{
	  fp = !fp;
	  printf("First Person %s.\n",fp?"on":"off");
	}
	if (key=='f')
		toggleHUD();
	if (key=='r')
		Island->Reset();
	if (key=='+')
		Island->setRenderDistance(Island->getRenderDistance()+Island->getRegionSize());
	if (key=='-')
		Island->setRenderDistance(Island->getRenderDistance()-Island->getRegionSize());	
	if (key=='t')		
		Island->renderDetail++;
	if (key=='g')		
		Island->renderDetail--;
	if (key=='x')		
		speed++;
	if (key=='z')		
		speed--;


	if (key=='h')	
	{
		float x,y,z;
		printf("Enter position to go to <x,y,z>: ");
		scanf("%f %f %f",&x,&y,&z);
		camera->SetX(x);
		camera->SetY(y);
		camera->SetZ(z);
		
	}
		
	 
}

void keyUp(unsigned char key,int x,int y)
{
	keyStates[key] = false;
}

void keyOperations(void)
{
	if (keyStates['a'])
	{
		camera->RotateX(-DownlookDist);
		camera->RotateY(0.75);
		camera->RotateX(DownlookDist);
	}
	if (keyStates['d'])
	{
		camera->RotateX(-DownlookDist);
		camera->RotateY(-0.75);
		camera->RotateX(DownlookDist);
	}
	if (keyStates['w'])
		camera->MoveForward((GLfloat)(-0.1* speed));
	if (keyStates['s'])
		camera->MoveForward((GLfloat)(0.1* speed ));
	if (keyStates['q'])
	{
		DownlookDist += 0.75;		
		camera->RotateX(0.75);
	}
	if (keyStates['e'])
	{
		DownlookDist -= 0.75;		
		camera->RotateX(-0.75);
	}
	if (keyStates[' '])
	  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
  	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	if (camera->GetX()<0)
		camera->SetX(0);
	if (camera->GetZ()<0)
		camera->SetZ(0);
	if (camera->GetX()>WORLD_SIZE-1)
		camera->SetX(WORLD_SIZE-1);
	if (camera->GetZ()>WORLD_SIZE-1)
		camera->SetZ(WORLD_SIZE-1);
  if (fp)
  {

  		
  		
  	
  	float newY = linearInterpolate(
  								linearInterpolate(
  																	Island->getSAt((int)camera->GetZ(),(int)camera->GetX())->elevation,
  																	Island->getSAt((int)camera->GetZ()+1,(int)camera->GetX())->elevation,
  																	camera->GetZ()-int(camera->GetZ())),
									linearInterpolate(
  																	Island->getSAt((int)camera->GetZ(),(int)camera->GetX()+1)->elevation,
  																	Island->getSAt((int)camera->GetZ()+1,(int)camera->GetX()+1)->elevation,
  																	camera->GetZ()-int(camera->GetZ())),
  								camera->GetX()-int(camera->GetX()));								

  	camera->SetY(newY+2);
  }
}

