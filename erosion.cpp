// URG  I hate this, but I am copying shamus here

#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "noise.h"
#include "erosion.h"
#include <stdlib.h>
#include <assert.h>

#define EROSION_SIZE 256

inline float abs(float a)
{
	return a>0?a:-a;
}
float* erosionData;

void initErosion()
{
	erosionData = new float[EROSION_SIZE*EROSION_SIZE];
	unsigned char* data = loadRaw("erode.raw",EROSION_SIZE);
	for (int i = 0;i<EROSION_SIZE;i++)
		for (int j = 0;j<EROSION_SIZE;j++)
			erosionData[i*EROSION_SIZE+j] = perlinNoise(float(i*0.2f)/EROSION_SIZE,float(j*0.2f)/EROSION_SIZE,9,0.95f);
	erode(erosionData,EROSION_SIZE);
	FILE* result = fopen("eroded.raw","wb");
	for (int i = 0;i<EROSION_SIZE*EROSION_SIZE;i++)
	{
		fprintf(result,"%c%c%c",char(256.0*erosionData[i]),char(256.0*erosionData[i]),char(256.0*erosionData[i]));
	}
	fclose(result);
	delete[] data;
}

int getIndex(int i,int j)
{
	if (i<0)
		i = EROSION_SIZE+i;
	if (j<0)
		j = EROSION_SIZE+j;
	i%= EROSION_SIZE;
	j%= EROSION_SIZE;
	return i*EROSION_SIZE+j;
}

float getErosionDatai(int x,int y)
{
	return erosionData[getIndex(x,y)];
}

float getErosionData(float x,float y)
{
	float data[4][5];
	for (int i = 0;i<4;i++)
		for (int j = 0;j<4;j++)
			data[i][j] = getErosionDatai((int)x+i-1,(int)y+j-1);
	for (int i = 0;i<4;i++)
		data[i][4] = cubicInterpolate(data[i][0],data[i][1],data[i][2],data[i][3],y-(int)y);
	return cubicInterpolate(data[0][4],data[1][4],data[2][4],data[3][4],x-(int)x);
}

void erode(float* data,int size)
{
  float min = data[0];
  float max = data[0];
  for (int y = 0; y < size; y++) 
    for (int x = 0; x < size; x++)
    {
      if (data[getIndex(y,x)]>min) 
        min = data[getIndex(y,x)];
	  if (data[getIndex(y,x)]<max)
		max = data[getIndex(y,x)];
	}
  for (int y = 0; y < size; y++) 
    for (int x = 0; x < size; x++)
      data[getIndex(y,x)] = (data[getIndex(y,x)]-min)/(max-min);



	float* buffer = new float[size * size];
  memcpy (buffer, data, sizeof (float) * size * size);
	
	for (int pass = 0;pass<0;pass++)
	{
		for (int y = 0;y<size;y++)
			for (int x = 0;x<size;x++)
			{
				float low;
				float high;
				int lowindexx = x;
				int lowindexy = y;
				low = high = data[getIndex(y,x)];
				int cx = x;
				int cy = y;
				while (1)
				{
					for (int nx = cx-1;nx<cx+2;nx++)
						for (int ny = cy-1;ny<cy+2;ny++)
						{
							if (data[getIndex(ny,nx)] < low)
							{
								low = data[getIndex(ny,nx)];
								lowindexx = nx;lowindexy=ny;
							}
						}
					if (lowindexx==cx && lowindexy==cy)
						break;
					buffer[getIndex(cy,cx)]*=0.98f;
					cx = lowindexx;cy=lowindexy;
				}
			}
	  memcpy (data, buffer, sizeof (float) * size * size);
	}		
	int k = 2;
	while (k--)
	{
		for (int y = 0; y < size; y++) 
	  {
	    for (int x = 0; x < size; x++)
	    {
	      float val = 0.0f;
	      int count = 0;
	      for (int nx = x-1; nx <= x+1; nx++)
	        for (int ny = y-1; ny <= y+1; ny++) 
	        {
	          val += data[getIndex(ny,nx)];
	          count++;
		      }
	      val /= (float)count;
	      data[getIndex(y,x)] = val;
	    }
	  }
	}
  min = data[0];
  max = data[0];
  for (int y = 0; y < size; y++) 
    for (int x = 0; x < size; x++)
    {
      if (data[getIndex(y,x)]>min) 
        min = data[getIndex(y,x)];
	  if (data[getIndex(y,x)]<max)
		max = data[getIndex(y,x)];
	}
  for (int y = 0; y < size; y++) 
    for (int x = 0; x < size; x++)
      data[getIndex(y,x)] = (data[getIndex(y,x)]-min)/(max-min);


	delete[] buffer;
}
