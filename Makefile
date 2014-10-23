CC = g++
CFLAGS = -Wall -I ./eigen/
LDFLAGS = stuff

RM = /bin/rm -f
all: main
main: RayTracer.cpp
	$(CC) $(CFLAGS) RayTracer.cpp -o raytracer

clean:
	rm -rf raytracer