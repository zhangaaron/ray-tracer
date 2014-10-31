#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>
#include "transformation.h"


void normalize(Vector3f* v){
	float len = sqrt(((*v)[0] * (*v)[0]) + ((*v)[1] * (*v)[1]) + ((*v)[2] * (*v)[2]));
	(*v)[0] = (*v)[0] / len;
	(*v)[1] = (*v)[1] / len;
	(*v)[2] = (*v)[2] / len;
}


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
		Transformation transformation;
		virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
		virtual bool intersectP(Ray& ray) = 0;
		virtual BRDF* get_material() = 0;
};
class Triangle : public Shape{
	public:
		Vector3f vertex1;
		Vector3f vertex2;
		Vector3f vertex3;
		BRDF* material;
		Triangle(Vector3f v1, Vector3f v2, Vector3f v3, BRDF* m);
		bool intersect(Ray& ray, float* thit, LocalGeo* local);
		bool intersectP(Ray& ray);
		BRDF* get_material();
	private:
};

Triangle::Triangle(Vector3f v1, Vector3f v2, Vector3f v3, BRDF* m){
	vertex1 = v1;
	vertex2 = v2;
	vertex3 = v3;
	material = m;
};

bool Triangle::intersect(Ray& ray, float* thit, LocalGeo* local){
	return false;
};


bool Triangle::intersectP(Ray& ray){
	return false;
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
	// transformation.translate(position);
	// transformation.scale()
};

BRDF* Sphere::get_material(){
	return material;
};

/* 	r(t) = e + d*t
	define P to be point on surface of sphere, and p to be the center of the sphere. 
	then: 

	(P - p) dot (P - p) - r^2 = 0

	set P = to r(t) and solve for t. This gives a quadratic equation with determinant. If t < 0, no real value of 
	t gives intersection.  
*/
bool Sphere::intersect(Ray& ray, float* thit, LocalGeo* local){

	//We want to translate the ray from world space into the object space, so apply inverse transformation. 
	Vector3f e = ray.pos * transformation.matrix_trans.inverse().linear(); //Don't scale the position of the ray!
	Vector3f d = transformation.matrix_trans.inverse() * ray.dir; //Gotta figure out inconsistent documentation here!

	Vector3f c = pos;

	float determinant = ((d.dot(e-c)) * (d.dot(e-c))) - (d.dot(d))*((e-c).dot(e-c) - radius*radius);
	if (determinant < 0){
		return false;
	}
	else{
		*thit = (-1 * (d.dot(e-c)) - sqrt(determinant)) / (d.dot(d)); //For what t value we get a hit. Always take negative value of det since its closer to viewer. 
		local->pos = *thit * d + e; 
		local->normal = (transformation.matrix_trans.linear().inverse().transpose() 
							* (local->pos - c)).normalized(); //This is a normalized vector transformed back into the world space!
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