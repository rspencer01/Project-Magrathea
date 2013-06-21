#include <GL/glut.h>
#include <stdio.h>


GLuint LoadTextureFromData(unsigned char* data,int width,int height)
{
    GLuint texture;

    glGenTextures( 1, &texture );
    glBindTexture(GL_TEXTURE_2D,texture);
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    return texture; //return whether it was successfull
}

GLuint LoadAlphaTextureFromData(unsigned char* data,int width,int height)
{
    GLuint texture;

    glGenTextures( 1, &texture );
    glBindTexture(GL_TEXTURE_2D,texture);
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return texture; //return whether it was successfull
}


GLuint LoadTextureFromFile( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data = new unsigned char[width * height * 3];
    FILE * file;

    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    fread( data, width * height * 3, 1, file );
    fclose( file );
    
    texture = LoadTextureFromData(data,width,height);
    delete [] data;
    return texture;
}

