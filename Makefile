CC = g++
CFLAGS = -g -Wall -ansi -I ./eigen/
LDFLAGS = stuff

RM = /bin/rm -f
all: main
main: RayTracer.cpp
	$(CC) $(CFLAGS) lodepng.cpp RayTracer.cpp -o raytracer

clean:
	rm -rf raytracer
	rm -rf helloworld.png