CC = g++
CFLAGS = -g -Wall -ansi -I ./eigen/ -O1
LDFLAGS = stuff

all: main
main: RayTracer.cpp
	$(CC) $(CFLAGS) lodepng.cpp RayTracer.cpp -o raytracer

clean:
	rm -rf raytracer
	rm -rf helloworld.png