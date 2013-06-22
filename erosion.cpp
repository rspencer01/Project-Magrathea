// URG  I hate this, but I am copying shamus here

#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "noise.h"
#include "erosion.h"
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
using namespace std;

#define EROSION_SIZE 256

float* erosionData;

void entropy_erode(float*);

void initErosion()
{
	erosionData = new float[EROSION_SIZE*EROSION_SIZE];
	unsigned char* data = loadRaw("erode.raw",EROSION_SIZE);
	for (int i = 0;i<EROSION_SIZE;i++)
		for (int j = 0;j<EROSION_SIZE;j++)
			erosionData[i*EROSION_SIZE+j] = data[(i*EROSION_SIZE+j)*3]/256.0;
	entropy_erode(erosionData);
	FILE* result = fopen("eroded.raw","wb");
	for (int i = 0;i<EROSION_SIZE*EROSION_SIZE;i++)
	{
		fprintf(result,"%c%c%c",char(255.0*erosionData[i]),char(255.0*erosionData[i]),char(255.0*erosionData[i]));
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

void entropy_erode (float* emap)
{

  float*  buffer;
  int     x, y;
  float   low, high, val;
  int currentx,currenty;
  int low_indexx,low_indexy, high_indexx, high_indexy;
  int nx,ny;
  int     index;
  int     count;

  buffer = new float[EROSION_SIZE * EROSION_SIZE];
  memcpy (buffer, emap, sizeof (float) * EROSION_SIZE * EROSION_SIZE);
  //Pass over the entire map, dropping a "raindrop" on each point. Trace
  //a path downhill until the drop hits bottom. Subtract elevation
  //along the way.  Makes natural hells from handmade ones. Super effective.
  for (int pass = 0; pass < 4; pass++) {
    for (y = 0; y < EROSION_SIZE; y++) {
      for (x = 0; x < EROSION_SIZE; x++) {
        low = high = buffer[x + y * EROSION_SIZE];
        currentx = x;
        currenty = y;
        low_indexx = high_indexx = currentx;
        low_indexy = high_indexy = currenty;
        while (1) {
          //look for neighbors lower than this point
          for (nx = currentx - 1; nx <= currentx + 1; nx++) {
            for (ny = currenty - 1; ny <= currenty + 1; ny++) {
              index = getIndex (nx,ny);
              if (emap[index] >= high) {
                high = emap[index];
                high_indexx = nx;
                high_indexy = ny;
              }
              if (emap[index] <= low) {
                low = emap[index];
                low_indexx = nx;
                low_indexy = ny;
              }
            }
          }
          //Search done.  
          //Sanity checks
          if (low_indexx < 0)
            low_indexx += EROSION_SIZE;
          if (low_indexy < 0)
            low_indexy += EROSION_SIZE;
          low_indexx %= EROSION_SIZE;
          low_indexy %= EROSION_SIZE;
          //If we didn't move, then we're at the lowest point
          if (low_indexx == currentx && low_indexy==currenty)
            break;
          index = getIndex (currentx,currenty);
          //If we're at the highest point around, we're on a spike.
          //File that sucker down.
          if (high_indexx == currentx && high_indexy==currenty)
            buffer[index] *= 0.95f;
          //Erode this point a tiny bit, and move down.
          buffer[index] *= 0.97f;
          currentx = low_indexx;
          currenty = low_indexy;
        }
      }
    }
    memcpy (emap, buffer, sizeof (float) * EROSION_SIZE * EROSION_SIZE);
  }

#define BLUR_RADIUS 2
  //Blur the elevations a bit to round off little spikes and divots.
  for (y = 0; y < EROSION_SIZE; y++) {
    for (x = 0; x < EROSION_SIZE; x++) {
      val = 0.0f;
      count = 0;
      for (nx = -BLUR_RADIUS; nx <= BLUR_RADIUS; nx++) {
        for (ny = -BLUR_RADIUS; ny <= BLUR_RADIUS; ny++) {
          currentx = ((x + nx) + EROSION_SIZE) % EROSION_SIZE;
          currenty = ((y + ny) + EROSION_SIZE) % EROSION_SIZE;
          index = getIndex (currentx,currenty);
          val += buffer[index];
          count++;
        }
      }
      val /= (float)count;
      emap[index] = (emap[index] + val) / 2.0f;
      emap[index] = val;
    }
  }
  
  
  delete[] buffer;
  //re-normalize the map

  high = 0;
  low = 999999;
  for (y = 0; y < EROSION_SIZE; y++) {
    for (x = 0; x < EROSION_SIZE; x++) {
      index = getIndex (x, y);
      high = max (emap[index], high);
      low = min (emap[index], low);
    }
  }
  high = high - low;
  for (y = 0; y < EROSION_SIZE; y++) {
    for (x = 0; x < EROSION_SIZE; x++) {
      index = getIndex (x, y);
      emap[index] -= low;
      emap[index] /= high;
    }
  }

}

