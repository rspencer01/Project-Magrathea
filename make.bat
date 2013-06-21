g++ -ggdb main.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o main.o
g++ -ggdb graphics.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o graphics.o
g++ -ggdb region.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o region.o
g++ -ggdb camera.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o camera.o
g++ -ggdb terrain.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o terrain.o
g++ -ggdb heightMap.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o heightMap.o
g++ -ggdb world.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o world.o
g++ -ggdb textures.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o textures.o 
g++ -ggdb noise.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o noise.o
g++ -ggdb misc.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o misc.o
g++ -ggdb erosion.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o erosion.o 
g++ -ggdb biome.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o biome.o
g++ -ggdb vector3.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o vector3.o
g++ -ggdb page.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o page.o
g++ -ggdb book.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o book.o
g++ -ggdb grass.cpp -c -I"C:\Program Files\Common Files\MinGW\freeglut\include" -o grass.o

g++  -o a.exe main.o graphics.o region.o camera.o terrain.o world.o heightMap.o misc.o textures.o noise.o erosion.o biome.o vector3.o page.o book.o grass.o -L"C:\Program Files\Common Files\MinGW\freeglut\lib" -lfreeglut -lopengl32 -lglu32