CPP=g++
FLAGS = -g
obj = camera.o heightmap.o biome.o noise.o vector3.o world.o graphics.o misc.o terrain.o textures.o erosion.o book.o page.o sky.o grass.o region.o main.o
lib = -lglut -lGLU -lGLEW

all: $(obj)
	$(CPP) $(obj) $(lib) $(FLAGS) -o a
	
clean:
	rm a
	rm *.o


%.o : %.cpp Makefile
	$(CPP) $< -o $@ -c $(lib) $(FLAGS)
