#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>
#include "sampler.h"
#include "transformation.h"
#include "geometry.h"
#include "camera.h"


using namespace std;
using namespace Eigen;
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
		float fall_off;
		PointLight(Vector3f pos, Vector3f lightColor, float fall_off);
		void generateLightRay(LocalGeo* local, Ray* lray);
		Vector3f getColor();
};


PointLight::PointLight(Vector3f position, Vector3f c, float fall_off){
	pos = position;
	lightColor = c;
	this->fall_off = fall_off;
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