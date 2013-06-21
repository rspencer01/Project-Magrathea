#ifndef MISC_H
#define MISC_H

void smooth(float* array,int w, int h,int r);
unsigned char* loadRaw(const char* fileName,int size);
float linearInterpolate(float,float,float);
float cosineInterpolate(float,float,float);
float cubicInterpolate(float,float,float,float,float);
float changeContrast(float value,float contrast);

#endif