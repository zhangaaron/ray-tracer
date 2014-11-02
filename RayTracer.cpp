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
#include "lighting.h"
#include "film.h"
#include "parser.h"
using namespace std;
using namespace Eigen;
using namespace lodepng;

void print_3f(Vector3f x){
	printf("%f, \t%f, \t%f\n", x[0], x[1], x[2]);
};



Intersection::Intersection(LocalGeo* l, Shape* s){
	localGeo = l;
	shape = s;
};

AggregatePrimitive::AggregatePrimitive() {

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




class RayTracer{
	public:
		AggregatePrimitive* objList;
		std::vector<Light*>* lightList;
		Vector3f ambient;
		RayTracer() {

		}
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
	rgb[0] += ambient[0] * constants->k_a[0];
	rgb[1] += ambient[1] * constants->k_a[1];
	rgb[2] += ambient[2] * constants->k_a[2];
	for(std::vector<Light*>::iterator it = lightList->begin(); it != lightList->end(); ++it) {

		Light* currentLight = (*it);

		LocalGeo geo_light_ray(local_pos, dummy);
		//Create a light ray from light to point of intersection
		currentLight->generateLightRay(&geo_light_ray, &lightRay);
		//Test to see if light is blocked, and if not, color the object.
		if (!(objList->intersectP(lightRay, hitObject))){


			Vector3f lightColor = currentLight->getColor();
			//rgb[0] += lightColor[0] * constants->k_a[0];
			//rgb[1] += lightColor[1] * constants->k_a[1];
			//rgb[2] += lightColor[2] * constants->k_a[2];

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
		struct parser_struct *init_params;
		Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList, Vector3f ambient,char *fileName);
		Scene(char *file_name);
		void render();
	private:
};

Scene::Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y, AggregatePrimitive* objlist, std::vector<Light*>* lightList, Vector3f ambient, char *fileName):
	mySampler(output_x, output_y), 
	myFilm(output_x, output_y, fileName),
  	myCamera(cam_coord, ll, lr, ul, ur, output_x, output_y),
	myTracer(objlist, lightList, ambient)
  	{
};


Scene::Scene(char *file_name){
	struct parser_struct *parser_params = (struct parser_struct *)malloc(sizeof(struct parser_struct));
	this->init_params = parser_params;
	parse_loop(file_name, parser_params);
	mySampler = Sampler(1000, 1000);
	myFilm = Film(1000, 1000, parser_params->file_name);
	myCamera = parser_params->camera;
	myTracer = RayTracer(&parser_params->obj_list, parser_params->light_list, parser_params->ambient_light);

};
void Scene::render() {
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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}




void test_transformations_triangles() {

	Vector3f cam_coord(0, 0, 150);
	Vector3f ll(-50, -50, 50);
	Vector3f lr(50, -50, 50);
	Vector3f ul(-50, 50, 50);
	Vector3f ur(50, 50, 50);


	//Sample material

	Vector3f k_a1(0.1, 0, 0);
	Vector3f k_d1(0.6, 0.3 , 0);
	Vector3f k_s1(1, 1, 1);
	Vector3f k_r1(0.6, 0, 0);
	BRDF testSphereColor1(k_a1, k_d1, k_s1, k_r1, 10);

	Vector3f a1(-10, -10, 0);
	Vector3f a2(-50, -20, 0);
	Vector3f a3(50, -50, 20);

	Vector3f b1(0, 50, 0);
	Vector3f b2(-50, -50, 0);
	Vector3f b3(50, -50, 0);

	Vector3f c1(0, 0, 78);
	Vector3f c2(45, 45, 0);
	Vector3f c3(45, -45, 0);


	Triangle testTriangle1(a1,a2,a3, &testSphereColor1);
	Triangle testTriangle2(b1, b2, b3, &testSphereColor1);
	Triangle testTriangle3(c1, c2, c3, &testSphereColor1);
	Vector3f up = Vector3f(0, 50, 0);
	Vector3f around = Vector3f(60, 50, 0);
	Vector3f scalingvector = Vector3f(3, 2, 1);
	testTriangle1.transformation.scale(scalingvector);
	//testTriangle1.transformation.rotate(around);
	testTriangle1.transformation.translate(up);

	std::vector<Shape*> objects;

	objects.push_back(&testTriangle1);
	// objects.push_back(&testTriangle2);
	// objects.push_back(&testTriangle3);

	AggregatePrimitive primitives(objects);



	Vector3f lightPos1(50, 0, 100);
	Vector3f lightColor1(1, 1, 1);

	Vector3f lightPos2(0, 0, -1);
	Vector3f lightColor2(0.4, 0.4, 0.4);


	PointLight point1(lightPos1, lightColor1, 0);
	DirectionalLight point2(lightPos2, lightColor2);
	std::vector<Light*> lightList;
	Vector3f ambient(0.3, 0.3, 0.3);
	lightList.push_back(&point1);
	lightList.push_back(&point2);


	char *output = "./helloworld.png";
	Scene myScene(cam_coord, ll, lr, ul, ur, 1000, 1000, &primitives, &lightList, ambient, output);

	myScene.render();


}

int main(int argc, char *argv[]) {

	//test_transformations_triangles();

	return 0;
}