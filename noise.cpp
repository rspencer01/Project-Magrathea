#include <stdio.h>

inline float abs(float x)
{
	return x>0?x:-x;
}

float random(int n)
{
  n=(n<<13)^n;
  int nn=(n*(n*n*15731+789221)+1376312589)&0x7fffffff;
  return abs(1.0-((double)nn/1073741824.0));
}


inline float noise(int x,int y)
{
	int n = x + y * 674506111;
  return random(n);
 }

inline float smoothNoise(int x, int y)
{
	float c = (noise(x-1,y-1)+noise(x+1,y-1)+noise(x+1,y-1)+noise(x-1,y+1))/16.0f; 
	float m = (noise(x-1,y)+noise(x+1,y)+noise(x,y-1)+noise(x,y+1))/8.0f;
	float e = noise(x,y);
	return e;
}

inline float interpolate(float y0,float y1,float y2,float y3,float x)
{
	float P = (y3-y2)-(y0-y1);
	float Q = (y0-y1)-P;
	float R = (y2-y0);
	float S = y1;
	return P*x*x*x+Q*x*x+R*x+S;
}

float interpolatedNoise(float x, float y)
{
	int ix = (int)x;
	float fx = x-ix;
	int iy = (int)y;
	float fy = y-iy;
  float a =  interpolate(noise(ix-1,iy-1),
 							     	 		  noise(ix,iy-1),
 								 		      noise(ix+1,iy-1),
 								 		      noise(ix+2,iy-1),fx); 
  float b =  interpolate(noise(ix-1,iy),
 							     	 		  noise(ix,iy),
 								 		      noise(ix+1,iy),
 								 		      noise(ix+2,iy),fx); 
  float c =  interpolate(noise(ix-1,iy+1),
 							     	 		  noise(ix,iy+1),
 								 		      noise(ix+1,iy+1),
 								 		      noise(ix+2,iy+1),fx); 
  float d =  interpolate(noise(ix-1,iy+2),
 							     	 		  noise(ix,iy+2),
 								 		      noise(ix+1,iy+2),
 								 		      noise(ix+2,iy+2),fx);
	return interpolate(a,b,c,d,fy);
}

float perlinNoise(float x,float y,int n,float p)
{
	float total = 0.0;
	float freq = 1.0;
	float amp = 1.0;
	float gs = 0.0;
	while (n--)
	{
		gs += amp;
		total = total + interpolatedNoise(x*freq,y*freq)*amp;
		amp *= p;
		freq *= 2;
	}
	return total/gs;
}