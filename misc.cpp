#include <stdio.h>
#include <math.h>
#include "misc.h"

void smooth(float* array,int w, int h, int r)
{
	float* temp = new float[w*h];
	for (int i = 0;i<w*h;i++)
			temp[i] = array[i];
	for (int i = r;i<h-r;i++)
		for (int j = r;j<w-r;j++)
			for (int dx = -r;dx<=r;dx++)
				for (int dy = -r;dy<=r;dy++)
					temp[i*w + j] += array[(i+dx)*h + (j+dy)];
	for (int i = 0;i<w*h;i++)
		array[i] = temp[i]/(r*r);
}

unsigned char* loadRaw(const char* fileName,int size)
{
	FILE* file = fopen( fileName, "rb" );
	if ( file == NULL ) return NULL;
	unsigned char* dat = new unsigned char[size*size*3];
	fread( dat, size * size * 3, 1, file );
	fclose(file);
	return dat;
}

float linearInterpolate(float x1,float x2,float t)
{
	return (1.f-t)*x1+t*x2;
}

float cosineInterpolate(float x1,float x2,float t)
{
	float x = (1.0-cos(t*3.141592))/2.0;
	return (1.0-x)*x1+x*x2;
}

//Scribes a cubic through the points.  X is fraction between v2 and v3
float cubicInterpolate(float v1,float v2,float v3,float v4,float x)
{
	float A = -0.5*v1 + 1.5*v2 - 1.5*v3 + 0.5*v4;
	float B = v1 - 2.5*v2 + 2*v3 - 0.5*v4;
	float C = -0.5*v1+0.5*v3;
	float D = v2;
	return A*x*x*x + B*x*x + C*x + D;
}

float changeContrast(float value,float contrast)
{
	if (value >= 0.5f)
	{
		float x = pow((value - 0.5f)*2,1.f-contrast);
		return 0.5f+ x/2.f;
	}
	else
	{
		float x = pow((0.5f-value)*2,1.f-contrast);
		return 0.5f - x/2.f;
	}
}