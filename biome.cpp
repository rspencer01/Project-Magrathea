#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
using namespace std;
#include "biome.h"
#include <math.h>
#include "noise.h"
#include "misc.h"
#include "erosion.h"

#define BIGSCALENUMBER 10

inline float interpolate(float a,float b,float x)
{
	return a*(1-x)+b*x;
}
//The world, for now is 100 biomes on a side
#define SIZE 100

inline float hump(float x)
{
	return (1.0f-cos(x*3.141592f*2.0f))/2.0f;
}

biomeSystem::biomeSystem()
{
	sizeInMetres = WORLD_SIZE;
	biomeSize = 32;
	sizeInBiomes = sizeInMetres/biomeSize+1;
	biomes = new biomeData*[sizeInBiomes];
	for(int i = 0;i<sizeInBiomes;i++)
		biomes[i] = new biomeData[sizeInBiomes];	
	for(int i = 0;i<sizeInBiomes;i++)
		for(int j = 0;j<sizeInBiomes;j++)
		{
			float d = pow((float(i-sizeInBiomes/2)/(sizeInBiomes/2))*(float(i-sizeInBiomes/2)/(sizeInBiomes/2)) + (float(j-sizeInBiomes/2)/(sizeInBiomes/2))*(float(j-sizeInBiomes/2)/(sizeInBiomes/2)),0.5f);
			biomes[i][j].baseLevel = 0.7f-d+getErosionData(i*2.0,j*2.0)*0.3;
		}	
	for(int i = 0;i<sizeInBiomes;i++)
		for(int j = 0;j<sizeInBiomes;j++)
		{
			biomes[i][j].moisture = (min(1.0,1.0f-float(j)/(sizeInBiomes)+0.2))*(0.75+float(abs(i-sizeInBiomes/2))/(sizeInBiomes)/2.0);
			biomes[i][j].temperature = (1.0f-float(i)/(sizeInBiomes));
			biomes[i][j].climate = CL_JUNGLE;
		}		
	for(int i = 0;i<sizeInBiomes;i++)
		for(int j = 0;j<sizeInBiomes;j++)
		{
			if (biomes[i][j].baseLevel<0)
				for (int di = max(0,i-1);di<min(i+2,sizeInBiomes);di++)
					for (int dj = max(0,j-1);dj<min(j+2,sizeInBiomes);dj++)
						biomes[di][dj].climate = CL_COASTAL;
		}		



	for(int i = 0;i<sizeInBiomes;i++)
		for(int j = 0;j<sizeInBiomes;j++)
		{
			biomes[i][j].grassColor[0] = linearInterpolate(142,linearInterpolate(13,202,1-biomes[i][j].moisture),min(1.f,biomes[i][j].temperature*3));
			biomes[i][j].grassColor[1] = linearInterpolate(207,linearInterpolate(157,80,1-biomes[i][j].moisture),min(1.f,biomes[i][j].temperature*3));
			biomes[i][j].grassColor[2] = linearInterpolate(213,linearInterpolate(26,48,1-biomes[i][j].moisture),min(1.f,biomes[i][j].temperature*3));
			
			biomes[i][j].grassColor[0] += rand()%30 - 15 + 255;biomes[i][j].grassColor[0]%=255;
			biomes[i][j].grassColor[1] += rand()%30 - 15 + 255;biomes[i][j].grassColor[1]%=255;
			biomes[i][j].grassColor[2] += rand()%10 - 5 + 255;biomes[i][j].grassColor[2]%=255;
			
			biomes[i][j].topography = (biomes[i][j].climate!=CL_COASTAL)?(rand()%100)/100.0f:0;
			biomes[i][j].detail     = (rand()%100)/100.0f;
			biomes[i][j].grassiness = hump(biomes[i][j].temperature)*biomes[i][j].moisture*biomes[i][j].moisture*hump(biomes[i][j].temperature)*biomes[i][j].moisture*biomes[i][j].moisture;
			biomes[i][j].grassHeight = hump(biomes[i][j].temperature)*biomes[i][j].moisture*biomes[i][j].moisture*hump(biomes[i][j].temperature)*biomes[i][j].moisture*biomes[i][j].moisture;
			
			biomes[i][j].flags = rand()%5;
		}	
	for (int k = 0;k<3;k++)
	for(int i = 1;i<sizeInBiomes-1;i++)
		for(int j = 1;j<sizeInBiomes-1;j++)			
		{
			biomes[i][j].topography = (biomes[i-1][j].topography+biomes[i-1][j-1].topography+biomes[i-1][j+1].topography+
																biomes[i][j].topography+biomes[i][j-1].topography+biomes[i][j+1].topography+
																biomes[i+1][j].topography+biomes[i+1][j-1].topography+biomes[i+1][j+1].topography)/9.0f;
			biomes[i][j].detail = (biomes[i-1][j].detail+biomes[i-1][j-1].detail+biomes[i-1][j+1].detail+
																biomes[i][j].detail+biomes[i][j-1].detail+biomes[i][j+1].detail+
																biomes[i+1][j].detail+biomes[i+1][j-1].detail+biomes[i+1][j+1].detail)/9.0f;		
		}
	float mn = biomes[0][0].topography;
	float mx = biomes[0][0].topography;
	for(int i = 1;i<sizeInBiomes-1;i++)
		for(int j = 1;j<sizeInBiomes-1;j++)	
		{
			mn = min(mn,biomes[i][j].topography);
			mx = max(mx,biomes[i][j].topography);
		}
	mx *= 0.99f;
	for(int i = 1;i<sizeInBiomes-1;i++)
		for(int j = 1;j<sizeInBiomes-1;j++)	
			biomes[i][j].topography = (biomes[i][j].topography-mn)/(mx-mn);

	FILE* biomeImage = fopen("biomes.raw","wb");
	unsigned char* data = new unsigned char[sizeInBiomes*sizeInBiomes*3];
	for(int i = 0;i<sizeInBiomes;i++)
		for(int j = 0;j<sizeInBiomes;j++)
		{
			if (biomes[i][j].baseLevel>0)
			{
				fprintf(biomeImage,"%c%c%c",biomes[i][j].grassColor[0],biomes[i][j].grassColor[1],biomes[i][j].grassColor[2]);
			}
			else
			{
				fprintf(biomeImage,"%c%c%c",0,0,255);
			}
				
		}	
	fclose(biomeImage);
	delete[] data;
}

biomeData* biomeSystem::getAt(int y,int x)
{
	y += noise(x*y,y)*10-5;
	x += noise(x*y,x)*10-5;
	int nx = x/biomeSize;
	int ny = y/biomeSize;
	//Do this check for heightmap.getat
	if (! ((nx>=0 && nx<sizeInBiomes) && (ny>=0 && ny<sizeInBiomes)))
	{
	//	fprintf(stderr,"Error: Biome request %d %d rectifying...\n",y,x);
	//assert(nx>=0 && nx<sizeInBiomes);
	//assert(ny>=0 && ny<sizeInBiomes);
		nx = max(0,min(nx,sizeInBiomes-1));
		ny = max(0,min(ny,sizeInBiomes-1));
	}
	return &biomes[nx][ny];
}
/*
int main()
{
	biomeSystem();
	
}*/