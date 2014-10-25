#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>
#include "lodepng.h"

using namespace std;
using namespace Eigen;
using namespace lodepng;

class Sample {
	public:
		int x, y;
		Sample(int x_in, int y_in);
};

Sample::Sample(int x_in, int y_in){
	x = x_in;
	y = y_in;
};

class Ray {
	public:
		Vector3f pos;
		Vector3f dir;
		float t_min, t_max;
		Ray(float min_t, float max_t);
		Ray(Vector3f pos_input, Vector3f dir_input, float min_t, float max_t);
	private:
};

Ray::Ray(float min_t, float max_t){
	t_min = min_t;
	t_max = max_t;
};

Ray::Ray(Vector3f pos_input, Vector3f dir_input, float min_t, float max_t){
	pos = pos_input;
	dir = dir_input;
	t_min = min_t;
	t_max = max_t;
};

class LocalGeo{
	public:
		Vector3f pos;
		Vector3f normal;
		LocalGeo(Vector3f position, Vector3f norm);
};

LocalGeo::LocalGeo(Vector3f position, Vector3f norm){
	pos = position;
	normal = norm;
};

class Shape{
	public:
		virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
		virtual bool intersectP(Ray& ray) = 0;
};

class Sphere : public Shape{
	public:
		float radius;
		Vector3f pos;
		Sphere(Vector3f position, float r);
		bool intersect(Ray& ray, float* thit, LocalGeo* local);
		bool intersectP(Ray& ray);
	private:
};

Sphere::Sphere(Vector3f position, float r){
	radius = r;
	pos = position;
};

bool Sphere::intersect(Ray& ray, float* thit, LocalGeo* local){
	Vector3f e = ray.pos;
	Vector3f d = ray.dir;

	Vector3f c = pos;

	float determinant = ((d.dot(e-c)) * (d.dot(e-c))) - (d.dot(d))*((e-c).dot(e-c) - radius*radius);
	if (determinant < 0){
		return false;
	}
	else{
		*thit = (-1 * (d.dot(e-c)) - sqrt(determinant)) / (d.dot(d));
		local->pos = *thit * d + e;
		local->normal = (local->pos - c)/radius;
		return true;
	}
};

bool Sphere::intersectP(Ray& ray){
	Vector3f e = ray.pos;
	Vector3f d = ray.dir;

	Vector3f c = pos;

	float determinant = ((d.dot(e-c)) * (d.dot(e-c))) - (d.dot(d))*((e-c).dot(e-c) - radius*radius);
	if (determinant < 0){
		return false;
	}else{
		return true;
	}
};

class Sampler {
	public:
		int x_max, y_max;
		int x, y;
		Sampler(int x_max, int y_max){
			this->x_max = x_max;
			this->y_max= y_max;
			x = 0;
			y = 0;
		};
		bool hasNext();
		void next(int *XYCoords);

	private:
};

bool Sampler::hasNext(){
	return (x < x_max && y < y_max);
}
void Sampler::next(int* XYCoords){	
	XYCoords[0] = x;
	XYCoords[1] = y;
	x = (x + 1) % x_max;
	if (x == 0)	y++;
};


//Bit packed struct for color, RGB values 0-255
struct Color {
	unsigned char R:8;
	unsigned char G:8;
	unsigned char B:8;
};
class Film {

	public:
		int width, height;
		char *fileName;
		unsigned char *RGBOutputArr;
		Film(int output_x, int output_y, char *fileName);
		void commit(int *XYCoords, Color *color);
		void writeImage();
	private:
};

Film::Film(int output_x, int output_y, char *fileName){

	width = output_x;
	height = output_y;
	this->fileName = fileName;
	RGBOutputArr = (unsigned char *)malloc(width * height * 3);
}

void Film::commit(int *XYCoords, Color *color){
	//Index into our output array and assign the correct color.
	int arrayLoctoWrite = (XYCoords[0] + XYCoords[1]) * 3;
	RGBOutputArr[arrayLoctoWrite] = color->R;
	RGBOutputArr[arrayLoctoWrite + 1] = color->G;
	RGBOutputArr[arrayLoctoWrite + 2] = color->B;
	//printf("%d, %d, %d, written at %d ", (int)color->R, (int)color->G, (int)color->B, arrayLoctoWrite);
}

void Film::writeImage(){

lodepng_encode24_file(fileName ,RGBOutputArr, width, height);
}

class Camera {
	public:
		Vector3f camera_coord;
		Vector3f ll;
		Vector3f lr;
		Vector3f ul;
		Vector3f ur;
		int output_x;
		int output_y;

		Camera(Vector3f coord, Vector3f lleft, Vector3f lright, Vector3f ulleft, Vector3f uright, int x, int y);
		void generateRay(int *XYCoords, Ray* ray);
	private:
};

Camera::Camera(Vector3f coord, Vector3f lleft, Vector3f lright, Vector3f ulleft, Vector3f uright, int x, int y){
	camera_coord = coord;
	ll = lleft;
	lr = lright;
	ul = ulleft;
	ur = uright;
	output_x = x;
	output_y = y;
};

void Camera::generateRay(int *XYCoords, Ray* ray){
	float u = XYCoords[0] / ((float)output_x);
	float v = XYCoords[1] / ((float)output_y);
	//printf("u: %d %f\tv: %d %f\n", sample.x, u, sample.y, v);
	Vector3f P = u*(v*ll + (1-v)* ul) + (1-u)*(v*lr + (1-v) * ur);
	ray->pos = camera_coord;
	ray->dir = P-camera_coord;
};




class Scene {
	public:
		Sampler mySampler;
		Film myFilm;
		Camera myCamera;

		Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, char *fileName);
		void render();
	private:
};

Scene::Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, 
			int output_x, int output_y, char *fileName) 
 : mySampler(output_x, output_y), myFilm(output_x, output_y, fileName),
  myCamera(cam_coord, ll, lr, ul, ur, output_x, output_y){

};

void Scene::render() {
	Sample sample (0,0);
	Ray ray(0, 1);
	while (mySampler.hasNext()) {
		int XYCoords[2];
		mySampler.next(XYCoords);
		myCamera.generateRay(XYCoords, &ray);
		// printf("Ray at X: %d Y: %d\n", sample.x, sample.y);
		printf("Pos: (%f, %f, %f)\tDirection: (%f, %f, %f)\n\n\n", ray.pos[0], ray.pos[1], ray.pos[2], ray.dir[0], ray.dir[1], ray.dir[2]);
		struct Color tempColor;
		tempColor.R = (char)(XYCoords[0] + XYCoords[1]);
		tempColor.G = (char)(XYCoords[0] + XYCoords[1]);
		tempColor.B = (char)(XYCoords[0] + XYCoords[1]);
		myFilm.commit(XYCoords, &tempColor);
		}
	myFilm.writeImage();
};

int main(int argc, char *argv[]) {
	Vector3f cam_coord(0, 0, 0);
	Vector3f ll(50, 100, -50);
	Vector3f lr(50, -100, -50);
	Vector3f ul(50, 100, 50);
	Vector3f ur(50, -100, 50);
	char *output = "./helloworld.png";
	Scene myScene(cam_coord, ll, lr, ul, ur, 200, 100, output);

	 myScene.render();
 	//unsigned char RGBOutputArr[] = {(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0};
	//lodepng_encode24_file("./hello" ,RGBOutputArr, 2, 2);

	return 0;
}