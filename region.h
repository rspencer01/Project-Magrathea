#ifndef REGION_H
#define REGION_H

class region;
class World;
#include <vector>
using namespace std;
#include "magrathea.h"
#include "terrain.h"
#include "world.h"
#include "vector3.h"
#include "grass.h"


#include <GL/glut.h>


class region
{
	private:
		//void smooth();
		Vector3 calcNorm(int,int);
		void CalculateNormals();
		void Triangulate(int);
		Vector3* normals;
		GLfloat* VertexData;
		GLfloat* NormalData;
		GLfloat* TextureData;
		GLfloat* ColourData;
		int** TriangleData;
		int* numTri;
		GLuint TextureNumber;
		World* parent;
		int bestDetail;
		sbit** surface;
		int getIndex(int,int);
		vector<grass> grasses;
	public:
		int x,y,randSeed;
		float* altitudes;
		int size;
		region(int,int,int,World*);
		~region();
		void Render(int);
		void RenderGrass(int);
		void populate();
};


#endif