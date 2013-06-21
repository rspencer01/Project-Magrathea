#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3
{
	public:
		float x;
		float y;
		float z;
		Vector3(float,float,float);
		//Vector3(int,int,int);
		Vector3();
		Vector3 operator=(const Vector3&);
		Vector3 operator+(Vector3);
		Vector3 operator-(Vector3);
		Vector3 operator*(float);
		float dot(Vector3);
		float norm();
		float getDimension(int);
		void normalise();
};

#endif
