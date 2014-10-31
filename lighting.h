#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>
#include "lodepng.h"
#include "sampler.h"
#include "transformation.h"
#include "geometry.h"
#include "camera.h"

using namespace std;



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
	lray->dir = pos;
};