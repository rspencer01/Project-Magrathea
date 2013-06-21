#ifndef MISC_H
#define MISC_H

void smooth(float* array,int w, int h,int r);
unsigned char* loadRaw(const char* fileName,int size);
float linearInterpolate(float x1,float x2,float t);
float cosineInterpolate(float x1,float x2,float t);

#endif