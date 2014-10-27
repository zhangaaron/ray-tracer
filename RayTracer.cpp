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

struct Color {
	float R;
	float G;
	float B;
};

class BRDF{
	public:
		Color* k_s;
		Color* k_a;
		Color* k_r;
		Color* k_d;
		float k_sp;
		BRDF(Color* a, Color* d, Color* s, Color* r, float sp);
	private:
};

BRDF::BRDF(Color* a, Color* d, Color* s, Color* r, float sp){
	k_s = s;
	k_a = a;
	k_r = r;
	k_d = d;
	k_sp = sp;
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
		BRDF* material;
		virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
		virtual bool intersectP(Ray& ray) = 0;
};

class Light{
	public:
		virtual void generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor) = 0;
	private:
};

class PointLight : public Light{
	public:
		Vector3f pos;
		Color* lightColor;
		PointLight(Vector3f pos, Color* c);
		void generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor);
};


PointLight::PointLight(Vector3f position, Color* c){
	pos = position;
	lightColor = c;
};

void PointLight::generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor){
	lray->pos = pos;
	lray->dir = local->pos - lray->pos;
	lcolor->R = lightColor->R;
	lcolor->G = lightColor->G;
	lcolor->B = lightColor->B;
	return;
};


class DirectionalLight : public Light{
	public:
		Vector3f pos;
		Color* lightColor;
		DirectionalLight(Vector3f pos, Color* c);
		void generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor);
};

DirectionalLight::DirectionalLight(Vector3f position, Color* c){
	pos = position;
	lightColor = c;
};

void DirectionalLight::generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor){
	lray->pos = local->pos;
	lray->dir = pos;
	lcolor->R = lightColor->R;
	lcolor->G = lightColor->G;
	lcolor->B = lightColor->B;
};

class AmbientLight{
	public:
		Color* lightColor;
		AmbientLight(Color* c);
		void generateLightRay(LocalGeo* local, Ray* lray, Color* lcolor);
};

AmbientLight::AmbientLight(Color* c){
	lightColor = c;
};

class Sphere : public Shape{
	public:
		float radius;
		Vector3f pos;
		BRDF* material;
		Sphere(Vector3f position, float r, BRDF* m);
		bool intersect(Ray& ray, float* thit, LocalGeo* local);
		bool intersectP(Ray& ray);
	private:
};


Sphere::Sphere(Vector3f position, float r, BRDF* m){
	radius = r;
	pos = position;
	material = m;
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

class Intersection{
	public:
		LocalGeo* localGeo;
		Shape* shape;
		Intersection(LocalGeo* l, Shape* s);
	private:
};

Intersection::Intersection(LocalGeo* l, Shape* s){
	localGeo = l;
	shape = s;
};


class AggregatePrimitive{
	public:
		vector<Shape*> list;
		AggregatePrimitive(vector<Shape*> l);
		bool intersect(Ray& ray, float* thit, Intersection* in, Shape* current);
		bool intersectP(Ray& ray, Shape* current);
	private:
};

AggregatePrimitive::AggregatePrimitive(vector<Shape*> l){
	list = l;
};



bool AggregatePrimitive::intersect(Ray& ray, float* thit, Intersection* in, Shape* current){
	float min_t = std::numeric_limits<float>::max();
	Vector3f holder1 (0,0,0);
	Vector3f holder2 (0,0,0);
	Shape* shapeholder = NULL;
	LocalGeo holder_local1(holder1, holder2);
	LocalGeo holder_local2(holder1, holder2);

	for(std::vector<Shape*>::iterator it = list.begin(); it != list.end(); ++it) {
    	Shape* holder_shape = (*it);
    	if (current != holder_shape){
    		if (holder_shape->intersect(ray, thit, &holder_local2)){
    			if (min_t > *thit){
	    			min_t = *thit;
	    			holder_local1 = holder_local2;
	    			shapeholder = holder_shape;
    			}
    		}
    	}
	}
	if (min_t != std::numeric_limits<float>::max()){
		*thit = min_t;
		in->localGeo = &holder_local1;
		in->shape = shapeholder;
		return true;
	}else{
		return false;
	}
};

bool AggregatePrimitive::intersectP(Ray& ray, Shape* current){
	for(std::vector<Shape*>::iterator it = list.begin(); it != list.end(); ++it) {
    	Shape* holder_shape = *it;
    	if (holder_shape != current){
    		if (holder_shape->intersectP(ray)){
    			return true;
    		}
    	}
	}
	return false;
} ;


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
	int arrayLoctoWrite = (XYCoords[0] + XYCoords[1] * width) * 3;
	RGBOutputArr[arrayLoctoWrite] = (int)((color->R)*255);
	RGBOutputArr[arrayLoctoWrite + 1] = (int)((color->G)*255);
	RGBOutputArr[arrayLoctoWrite + 2] = (int)((color->B)*255);
	//printf("%d, %d, %d, written at %d\n", (int)color->R, (int)color->G, (int)color->B, arrayLoctoWrite);
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

class RayTracer{
	public:
		AggregatePrimitive* objList;
		std::vector<Light*>* lightList;
		RayTracer(AggregatePrimitive* o, std::vector<Light*>* l);
		void trace(Ray& ray, int depth, Color* color);
	private:
};

RayTracer::RayTracer(AggregatePrimitive* o, std::vector<Light*>* l){
	objList = o;
	lightList = l;
}

void RayTracer::trace(Ray& ray, int depth, Color* color){
	if (depth > 6){
		color->R = 0;
		color->G = 0;
		color->B = 0;
		return ;
	}
	if(!(objList->intersectP(ray, NULL))){
		color->R = 0;
		color->G = 0;
		color->B = 0;
		return ;
	}

	float holder = 0;
	Vector3f dummy(0,0,0);
	LocalGeo holderGeo(dummy, dummy);
	Shape* shapeHolder = NULL;
	Intersection in(&holderGeo, shapeHolder);
	objList->intersect(ray, &holder, &in, NULL);
	Shape* hitObject = in.shape;
	Ray lightRay(0, 1);
	struct Color lightColor;
	printf("we got here");
	for(std::vector<Light*>::iterator it = lightList->begin(); it != lightList->end(); ++it) {
		Light* currentLight = (*it);
		currentLight->generateLightRay(in.localGeo, &lightRay, &lightColor);
		if (!(objList->intersectP(lightRay, hitObject))){
			hitObject->material;
			Vector3f rgb(0,0,0);


		}

	}


};


class Scene {
	public:
		Sampler mySampler;
		Film myFilm;
		Camera myCamera;
		RayTracer myTracer;
		Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList,char *fileName);
		void render();
	private:
};

Scene::Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList, char *fileName):
	mySampler(output_x, output_y), myFilm(output_x, output_y, fileName),
  	myCamera(cam_coord, ll, lr, ul, ur, output_x, output_y),
	myTracer(objlist, lightList)
  	{
};

void Scene::render() {
	Sample sample (0,0);
	Ray ray(0, 1);
	while (mySampler.hasNext()) {
		int XYCoords[2];
		mySampler.next(XYCoords);
		myCamera.generateRay(XYCoords, &ray);
		// printf("Ray at X: %d Y: %d\n", sample.x, sample.y);
		//printf("Pos: (%f, %f, %f)\tDirection: (%f, %f, %f)\n\n\n", ray.pos[0], ray.pos[1], ray.pos[2], ray.dir[0], ray.dir[1], ray.dir[2]);
		struct Color tempColor;
		tempColor.R = 0;
		tempColor.G = 0;
		tempColor.B = 0;
		myTracer.trace(ray, 0, &tempColor);

		printf("X:%d\tY:%d\tR:%f\tG:%f\tB:%f \n", XYCoords[0], XYCoords[1],tempColor.R, tempColor.G, tempColor.B);
		myFilm.commit(XYCoords, &tempColor);
	}
	myFilm.writeImage();
};

int main(int argc, char *argv[]) {
	Vector3f cam_coord(0, 0, 100);
	Vector3f ll(-50, -50, 0);
	Vector3f lr(50, -50, 0);
	Vector3f ul(-50, 50, 0);
	Vector3f ur(50, 50, 0);
	Vector3f pos(0, 0, -50);

	struct Color k_a;
	k_a.R = 0.1;
	k_a.G = 0.1;
	k_a.B = 0;
	struct Color k_d;
	k_a.R = 1;
	k_a.G = 1;
	k_a.B = 0;
	struct Color k_s;
	k_a.R = 0;
	k_a.G = 0.8;
	k_a.B = 0.8;
	struct Color k_r;
	k_a.R = 0;
	k_a.G = 0;
	k_a.B = 0;
	BRDF testSphereColor(&k_a, &k_d, &k_s, &k_r, 16);
	Sphere testSphere(pos, 45, &testSphereColor);

	std::vector<Shape*> objects;
	objects.push_back(&testSphere);
	AggregatePrimitive primitives(objects);
	std::vector<Light*> lightList;

	char *output = "./helloworld.png";
	Scene myScene(cam_coord, ll, lr, ul, ur, 100, 100, &primitives, &lightList, output);

	myScene.render();
 	unsigned char RGBOutputArr[] = {(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0};
	lodepng_encode24_file("./hello" ,RGBOutputArr, 2, 2);

	return 0;
}