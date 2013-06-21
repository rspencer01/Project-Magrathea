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
/*
float tripleInterpolate(float x1,float x2, float x2, float t)
{
	if (t<1)
		return cosineInterpolate(x1,x2,t);
	return cosineInterpolate(x2,x3,t-1.0);
}
*/