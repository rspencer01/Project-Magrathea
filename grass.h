#ifndef GRASS_H
#define GRASS_H

#include "magrathea.h"
#include "vector3.h"
class grass;
#include "world.h"
//A helper class that stores info about and renders grass

class grass
{
	private:
		Vector3 position;
		Vector3 normal;
		World* parent;
		float height;
	public:
		grass(Vector3,Vector3,World*,float);
		void Render();
};

#endif