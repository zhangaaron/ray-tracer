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

void print_3f(Vector3f x){
	printf("%f, \t%f, \t%f\n", x[0], x[1], x[2]);
};

class Sample {
	public:
		int x, y;
		Sample(int x_in, int y_in);
};

Sample::Sample(int x_in, int y_in){
	x = x_in;
	y = y_in;
};

class BRDF{
	public:
		Vector3f k_s;
		Vector3f k_a;
		Vector3f k_r;
		Vector3f k_d;
		float k_sp;
		BRDF(Vector3f a, Vector3f d, Vector3f s, Vector3f r, float sp);
		void print();
	private:
};


BRDF::BRDF(Vector3f a, Vector3f d, Vector3f s, Vector3f r, float sp){
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
		virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
		virtual bool intersectP(Ray& ray) = 0;
		virtual BRDF* get_material() = 0;
};

class Light{
	public:
		virtual void generateLightRay(LocalGeo* local, Ray* lray) = 0;
		virtual Vector3f getColor() = 0;
	private:
};

class PointLight : public Light{
	public:
		Vector3f pos;
		Vector3f lightColor;
		PointLight(Vector3f pos, Vector3f lightColor);
		void generateLightRay(LocalGeo* local, Ray* lray);
		Vector3f getColor();
};


PointLight::PointLight(Vector3f position, Vector3f c){
	pos = position;
	lightColor = c;
};

void PointLight::generateLightRay(LocalGeo* local, Ray* lray){
	lray->pos = local->pos;
	lray->dir = pos - local->pos;
	return;
};

Vector3f PointLight::getColor(){
	return lightColor;
}

class DirectionalLight : public Light{
	public:
		Vector3f pos;
		Vector3f lightColor;
		DirectionalLight(Vector3f pos, Vector3f c);
		void generateLightRay(LocalGeo* local, Ray* lray);
		Vector3f getColor();

};

DirectionalLight::DirectionalLight(Vector3f position, Vector3f c){
	pos = position;
	lightColor = c;
};

Vector3f DirectionalLight::getColor(){
	return lightColor;
}

void DirectionalLight::generateLightRay(LocalGeo* local, Ray* lray){
	lray->pos = local->pos;
	lray->dir = -1 * pos;
};

/*
class AmbientLight{
	public:
		Vector3f lightColor;
		AmbientLight(Color* c);
};

AmbientLight::AmbientLight(Vector3f c){
	lightColor = c;
};*/

class Triangle : public Shape{
	public:
		Vector3f v1;
		Vector3f v2;
		Vector3f v3;
		BRDF* material;
		Triangle(Vector3f vertex1, Vector3f vertex2, Vector3f vertex3, BRDF* m);
		bool intersect(Ray& ray, float* thit, LocalGeo* local);
		bool intersectP(Ray& ray);
		BRDF* get_material();
	private:
};

Triangle::Triangle(Vector3f vertex1, Vector3f vertex2, Vector3f vertex3, BRDF* m){
	v1 = vertex1;
	v2 = vertex2;
	v3 = vertex3;
	material = m;
};

bool Triangle::intersect(Ray& ray, float* thit, LocalGeo* local){
	Vector3f A = v2-v1;
	Vector3f B = v3-v1;
	//printf(".");
	Vector3f N = A.cross(B);

	float n_dot_ray = N.dot(ray.dir);
	//printf("Cross direction:\t%f\t%f\t%f\n", N[0], N[1], N[2]);
	//printf("Ray direction:  \t%f\t%f\t%f\n\n", ray.dir[0], ray.dir[1], ray.dir[2]);
	//Parallel Case
	if(n_dot_ray == 0){
		//printf("case 1 fail");
		return false;
	}
	float d = N.dot(v1);
	float t = -(N.dot(ray.pos) + d) / n_dot_ray;
	//printf("\nt:\t%f\n", n_dot_ray);
	Vector3f point = t*ray.dir + ray.pos;

	if (N.dot(A.cross(point - v1)) < 0){
		//printf("case 2 fail");
		return false;
	}

	if (N.dot((v3-v2).cross(point - v2)) < 0){
		//printf("case 3 fail");
		return false;
	}

	if (N.dot((v1-v3).cross(point - v3)) < 0){
		//printf("case 4 fail");
		return false;
	}

	*thit = t;
	local->pos = point;
	local->normal = N;
	//printf("Ray direction:  \t%f\t%f\t%f\n\n", N[0], N[1], N[2]);
	//printf("\nhit\n");
	return true;
};


bool Triangle::intersectP(Ray& ray){
	Vector3f A = v2-v1;
	Vector3f B = v3-v1;

	Vector3f N = A.cross(B);
	float n_dot_ray = N.dot(ray.dir);
	//Parallel Case
	if(n_dot_ray == 0){
		return false;
	}
	float d = N.dot(v1);
	float t = -(N.dot(ray.pos) + d) / n_dot_ray;

	Vector3f point = t*ray.dir + ray.pos;
	if(t < 0.00000000000001){
		return false;
	}
	if (N.dot(A.cross(point - v1)) < 0){
		return false;
	}

	if (N.dot((v3-v2).cross(point - v2)) < 0){
		return false;
	}

	if (N.dot((v1-v3).cross(point - v3)) < 0){
		return false;
	}

	return true;
};

BRDF* Triangle::get_material(){
	return material;
};




class Sphere : public Shape{
	public:
		float radius;
		Vector3f pos;
		BRDF* material;
		Sphere(Vector3f position, float r, BRDF* m);
		bool intersect(Ray& ray, float* thit, LocalGeo* local);
		bool intersectP(Ray& ray);
		BRDF* get_material();
	private:
};


Sphere::Sphere(Vector3f position, float r, BRDF* m){
	radius = r;
	pos = position;
	material = m;
};

BRDF* Sphere::get_material(){
	return material;
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
	if (determinant < 0 || ((-1 * (d.dot(e-c)) - sqrt(determinant)) / (d.dot(d))) < -0.0001 ){
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
		void commit(int *XYCoords, Vector3f color);
		void writeImage();
	private:
};

Film::Film(int output_x, int output_y, char *fileName){
	width = output_x;
	height = output_y;
	this->fileName = fileName;
	RGBOutputArr = (unsigned char *)malloc(width * height * 3);
}

void Film::commit(int *XYCoords, Vector3f color){
	int arrayLoctoWrite = (XYCoords[0] + XYCoords[1] * width) * 3;
	RGBOutputArr[arrayLoctoWrite] = (int)round((color[0])*255);
	RGBOutputArr[arrayLoctoWrite + 1] = (int)round((color[1])*255);
	RGBOutputArr[arrayLoctoWrite + 2] = (int)round((color[2])*255);
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
	Vector3f P = (1-u)*(v*ll + (1-v)* ul) + (u)*(v*lr + (1-v) * ur);
	ray->pos = camera_coord;
	ray->dir = P - camera_coord;
};

class RayTracer{
	public:
		AggregatePrimitive* objList;
		std::vector<Light*>* lightList;
		Vector3f ambient;
		RayTracer(AggregatePrimitive* o, std::vector<Light*>* l, Vector3f a);
		void trace(Ray& ray, int depth, Vector3f* color);
	private:
};

RayTracer::RayTracer(AggregatePrimitive* o, std::vector<Light*>* l, Vector3f a){
	objList = o;
	lightList = l;
	ambient = a;
}

void setColor(Vector3f* color, float r, float g, float b){

	(*color)[0] = std::min(r, 1.0f);
	(*color)[1] = std::min(g, 1.0f);
	(*color)[2] = std::min(b, 1.0f);
}

void normalize(Vector3f* v){
	float len = sqrt(((*v)[0] * (*v)[0]) + ((*v)[1] * (*v)[1]) + ((*v)[2] * (*v)[2]));
	(*v)[0] = (*v)[0] / len;
	(*v)[1] = (*v)[1] / len;
	(*v)[2] = (*v)[2] / len;
}

void RayTracer::trace(Ray& ray, int depth, Vector3f* color){
	if (depth > 6){
		setColor(color, 0, 0, 0);
		return ;
	}
	if(!(objList->intersectP(ray, NULL))){
		setColor(color, 0, 0, 0);
		return ;
	}

	float holder = 0;
	Vector3f dummy(0,0,0);

	LocalGeo holderGeo(dummy, dummy);
	Shape* shapeHolder = NULL;

	Intersection in(&holderGeo, shapeHolder);
	objList->intersect(ray, &holder, &in, NULL);

	//Glitches out if you don't put here
	Vector3f surface_normal(in.localGeo->normal[0], in.localGeo->normal[1], in.localGeo->normal[2]);
	Vector3f surface_normal2(in.localGeo->normal[0], in.localGeo->normal[1], in.localGeo->normal[2]);
	Shape* hitObject = in.shape;
	Ray lightRay(0, 1);
	Vector3f local_pos(in.localGeo->pos[0], in.localGeo->pos[1], in.localGeo->pos[2]);
	BRDF* constants = (hitObject)->get_material();
	Vector3f rgb(0,0,0);
	//rgb[0] += ambient[0] * constants->k_a[0];
	//rgb[1] += ambient[1] * constants->k_a[1];
	//rgb[2] += ambient[2] * constants->k_a[2];
	for(std::vector<Light*>::iterator it = lightList->begin(); it != lightList->end(); ++it) {

		Light* currentLight = (*it);

		LocalGeo geo_light_ray(local_pos, dummy);
		currentLight->generateLightRay(&geo_light_ray, &lightRay);

		if (!(objList->intersectP(lightRay, hitObject))){


			Vector3f lightColor = currentLight->getColor();
			rgb[0] += lightColor[0] * constants->k_a[0];
			rgb[1] += lightColor[1] * constants->k_a[1];
			rgb[2] += lightColor[2] * constants->k_a[2];

			//printf("Colored ambient in things with %f, %f, %f\n", rgb[0], rgb[1], rgb[2]);
			//Diffuse term calculation

			normalize(&surface_normal);

			Vector3f light_direction = lightRay.dir;
			normalize(&light_direction);

			Vector3f product_term_d = (lightColor * std::max(surface_normal.dot(light_direction), 0.0f));
			rgb[0] += product_term_d[0] * constants->k_d[0];
			rgb[1] += product_term_d[1] * constants->k_d[1];
			rgb[2] += product_term_d[2] * constants->k_d[2];


			//Specular term calculation
			//printf("Surface Norm:\t%f,\t%f,\t%f\n", surface_normal[0], surface_normal[1], surface_normal[2]);
			//printf("Light Direct:\t%f,\t%f,\t%f\n", light_direction[0], light_direction[1], light_direction[2]);
			//printf("Dot: %f\n\n", surface_normal.dot(light_direction));
			Vector3f reflected = lightRay.dir - 2*(lightRay.dir.dot(surface_normal)) * surface_normal;
			normalize(&reflected);
			Vector3f viewer = -1 * ray.dir;
			normalize(&viewer);
			Vector3f product_term_s = lightColor * std::pow(std::max((-1*reflected).dot(viewer), 0.0f), constants->k_sp);
			rgb[0] += product_term_s[0] * constants->k_s[0];
			rgb[1] += product_term_s[1] * constants->k_s[1];
			rgb[2] += product_term_s[2] * constants->k_s[2];
			//printf("Reflected %f, %f, %f\n", reflected[0], reflected[1], reflected[2]);
			//printf("Surface Norm %f, %f, %f\n", surface_normal[0], surface_normal[1], surface_normal[2]);
			//printf("dot %f\n\n", (reflected.dot(surface_normal)));
		}
	}
	if(constants->k_r[0] > 0 && constants->k_r[1] > 0 && constants->k_r[2] > 0){
		normalize(&surface_normal2);
		Vector3f reflected_direction = ray.dir - 2*(ray.dir.dot(surface_normal2)) * surface_normal2;
		Ray reflected_ray(0, 1);
		reflected_ray.pos = local_pos;
		reflected_ray.dir = -1*reflected_direction;
		Vector3f tempColor(0,0,0);
		trace(reflected_ray, depth+1, &tempColor);
		rgb[0] += constants->k_r[0] * tempColor[0];
		rgb[1] += constants->k_r[1] * tempColor[1];
		rgb[2] += constants->k_r[2] * tempColor[2];
	}
	setColor(color, rgb[0], rgb[1], rgb[2]);
};

class Scene {
	public:
		Sampler mySampler;
		Film myFilm;
		Camera myCamera;
		RayTracer myTracer;
		Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList, Vector3f ambient,char *fileName);
		void render();
	private:
};

Scene::Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList, Vector3f ambient, char *fileName):
	mySampler(output_x, output_y), myFilm(output_x, output_y, fileName),
  	myCamera(cam_coord, ll, lr, ul, ur, output_x, output_y),
	myTracer(objlist, lightList, ambient)
  	{
};

void Scene::render() {
	Sample sample (0,0);
	Ray ray(0, 1);
	while (mySampler.hasNext()) {
		int XYCoords[2];
		mySampler.next(XYCoords);
		myCamera.generateRay(XYCoords, &ray);
		Vector3f tempColor(0,0,0);
		myTracer.trace(ray, 0, &tempColor);
		myFilm.commit(XYCoords, tempColor);
	}
	myFilm.writeImage();
};

int main(int argc, char *argv[]) {

	//Making camera
	Vector3f cam_coord(0, 0, 150);
	Vector3f ll(-50, -50, 50);
	Vector3f lr(50, -50, 50);
	Vector3f ul(-50, 50, 50);
	Vector3f ur(50, 50, 50);

	Vector3f pos1(0, 50, 0);
	Vector3f pos2(-50, -50, 0);
	Vector3f pos3(50, -50, 0);

	//Sample material

	Vector3f k_a1(0.1, 0.1, 0);
	Vector3f k_d1(1, 1, 0);
	Vector3f k_s1(0.8, 0.8, 0.8);
	Vector3f k_r1(0, 0, 0);

	Vector3f k_a2(0, 0.1, 0.1);
	Vector3f k_d2(0, 0.4, 0.4);
	Vector3f k_s2(0, 0.8, 0.8);
	Vector3f k_r2(0, 0, 0);

	Vector3f k_a3(0.2, 0.2, 0.2);
	Vector3f k_d3(0.3, 0.3, 0.3);
	Vector3f k_s3(0.5, 0.5, 0.5);
	Vector3f k_r3(0, 0, 0);

	BRDF testSphereColor1(k_a2, k_d2, k_s2, k_r2, 1000);
	BRDF testSphereColor2(k_a3, k_d3, k_s3, k_r3, 20);


	Vector3f pos5(-30, 0, -100);
	Vector3f pos6(30, 0, -50);
	//Sampe sphere
	//Sphere testSphere1(pos2, 25, &testSphereColor1);
	Sphere testSphere1(pos1, 10, &testSphereColor2);
	Sphere testSphere2(pos2, 10, &testSphereColor2);
	Sphere testSphere3(pos3, 10, &testSphereColor2);
	Triangle testTriangle1(pos1, pos2, pos3, &testSphereColor1);


	//Add objects here
	std::vector<Shape*> objects;
	objects.push_back(&testSphere1);
	objects.push_back(&testSphere2);
	objects.push_back(&testSphere3);
	objects.push_back(&testTriangle1);

	Vector3f lightPos1(200, 200, 200);
	Vector3f lightColor1(0.7, 0.7, 0.7);

	Vector3f lightPos2(0, 0, -1);
	Vector3f lightColor2(0.4, 0.4, 0.4);


	PointLight point1(lightPos1, lightColor1);
	DirectionalLight point2(lightPos2, lightColor2);
	//Add Lights here
	AggregatePrimitive primitives(objects);
	std::vector<Light*> lightList;
	lightList.push_back(&point1);
	lightList.push_back(&point2);

	Vector3f ambient(0.3, 0.3, 0.3);

	char *output = "./helloworld.png";
	Scene myScene(cam_coord, ll, lr, ul, ur, 1000, 1000, &primitives, &lightList, ambient, output);

	myScene.render();
 	unsigned char RGBOutputArr[] = {(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0};
	lodepng_encode24_file("./hello" ,RGBOutputArr, 2, 2);

	return 0;
	/*
	//Making camera
	Vector3f cam_coord(0, 0, 100);
	Vector3f ll(-50, -50, 0);
	Vector3f lr(50, -50, 0);
	Vector3f ul(-50, 50, 0);
	Vector3f ur(50, 50, 0);
	Vector3f pos1(0, 0, -50);

	Vector3f pos2(-30, 0, -50);
	Vector3f pos3(30, 0, -50);
	Vector3f pos4(0, 0, -200);

	//Sample material
	Vector3f k_a1(0.1, 0.1, 0);
	Vector3f k_d1(1, 1, 0);
	Vector3f k_s1(0.8, 0.8, 0.8);
	Vector3f k_r1(0, 0, 0);

	Vector3f k_a2(0.1, 0, 0);
	Vector3f k_d2(1, 0, 0);
	Vector3f k_s2(0.8, 0.8, 0.8);
	Vector3f k_r2(0.5, 0.5, 0.5);

	Vector3f k_a3(0, 0.3, 0.3);
	Vector3f k_d3(0, 1, 1);
	Vector3f k_s3(0.8, 0.8, 0.8);
	Vector3f k_r3(0, 0, 0);
	BRDF testSphereColor1(k_a2, k_d2, k_s2, k_r2, 10);
	BRDF testSphereColor2(k_a3, k_d3, k_s3, k_r3, 10);

	Vector3f pos5(-30, 0, -100);
	Vector3f pos6(30, 0, -50);
	//Sampe sphere
	//Sphere testSphere1(pos2, 25, &testSphereColor1);
	Sphere testSphere2(pos5, 30, &testSphereColor1);
	Sphere testSphere3(pos6, 30, &testSphereColor2);
	//Add objects here
	std::vector<Shape*> objects;
	//objects.push_back(&testSphere1);
	objects.push_back(&testSphere2);
	objects.push_back(&testSphere3);

	Vector3f lightPos1(200, 200, 200);
	Vector3f lightColor1(0.3, 0.3, 0.3);

	Vector3f lightPos2(0, 100, -150);
	Vector3f lightColor2(0.5, 0.5, 0.5);


	PointLight point1(lightPos1, lightColor1);
	PointLight point2(lightPos2, lightColor2);
	//Add Lights here
	AggregatePrimitive primitives(objects);
	std::vector<Light*> lightList;
	lightList.push_back(&point1);
	lightList.push_back(&point2);

	Vector3f ambient(0.3, 0.3, 0.3);

	char *output = "./helloworld.png";
	Scene myScene(cam_coord, ll, lr, ul, ur, 2000, 2000, &primitives, &lightList, ambient, output);

	myScene.render();
 	unsigned char RGBOutputArr[] = {(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0,(char)255, (char)0, (char)0};
	lodepng_encode24_file("./hello" ,RGBOutputArr, 2, 2);

	return 0;*/
}