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
		bool finishedTexture;
		void doNextTexture();
		void doPatch(int,int);
		int texX;
		int texY;
		int patch_steps;
		int _patch_size;
		GLuint vertexVBO;
		GLuint textureVBO;
	public:
		int origin_x,origin_y,randSeed;
		float* altitudes;
		int size;
		region(int,int,int,World*);
		~region();
		void Render(int);
		void RenderGrass(int);
		void populate();
};


#endif