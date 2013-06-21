CPP=g++
obj = main.o camera.o heightmap.o region.o biome.o noise.o vector3.o world.o graphics.o misc.o terrain.o textures.o erosion.o
lib = -lglut -lGLU

all: $(obj)
	$(CPP) $(obj) $(lib) -o a
	

%.o : %.c Makefile
	$(CPP) $< -c -o $@ $(lib)
