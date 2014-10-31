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
		Transformation trans;
		virtual bool intersect(Ray& ray, float* thit, LocalGeo* local) = 0;
		virtual bool intersectP(Ray& ray) = 0;
		virtual BRDF* get_material() = 0;
};

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
	Vector3f N = (A.cross(B));

	float n_dot_ray = N.dot(ray.dir);
	//printf("Cross direction:\t%f\t%f\t%f\n", N[0], N[1], N[2]);
	//printf("Ray direction:  \t%f\t%f\t%f\n\n", ray.dir[0], ray.dir[1], ray.dir[2]);
	//Parallel Case
	if(n_dot_ray == 0){
		//printf("case 1 fail");
		return false;
	}
	float d = N.dot(v1);
	float t = -1*((ray.pos - v1).dot(N) / n_dot_ray);
	//float t = -(N.dot(ray.pos) + d) / n_dot_ray;
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
	float t = -1*((ray.pos - v1).dot(N) / n_dot_ray);
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
