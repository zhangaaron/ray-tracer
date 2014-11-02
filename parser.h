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
#include "lighting.h"

using namespace std;
using namespace Eigen;
using namespace lodepng;
 /*

 This file can be used to parse input files given to our ray tracer and return an a struct containing all the information to set up a scene. 
 */

enum TRANSFORMATION {
	TRANSLATION,
	ROTATION, 
	SCALE
};
//This gets filled out by the parse_loop
struct parser_struct {
	Camera camera;
	AggregatePrimitive obj_list;
	std::vector<Light*> *light_list;
	Vector3f ambient_light;
	char * file_name;
};

//For a space seperated line consisting of numbers, convert every input into integer and put into fill array. 
void fill_param(int* fill, int size) {
		for (int i = 0; i < size; i++) {
			char *token = strtok(NULL, " ");
			if (token == NULL) {
				printf("Malformed string in scene file, exiting. \n");
			}
			fill[i] = atoi(token);
		}
}

//For a space seperated line consisting of numbers, convert every input into float and put into fill array. 
void fill_param_float(float* fill, int size) {
		for (int i = 0; i < size; i++) {
			char *token = strtok(NULL, " ");
			if (token == NULL) {
				printf("Malformed string in scene file, exiting. \n");
			}
			fill[i] = atof(token);
		}
}

Camera parse_camera(char *line) {
	int param[15];
	fill_param(param, 15);
	Vector3f eye_coords = Vector3f(param[0], param[1], param[2]);
	Vector3f LL_coords = Vector3f(param[3], param[4], param[5]);
	Vector3f LR_coords = Vector3f(param[6], param[7], param[8]);
	Vector3f UL_coords = Vector3f(param[9], param[10], param[11]);
	Vector3f UR_coords = Vector3f(param[12], param[13], param[14]);
	return Camera(eye_coords, LL_coords, LR_coords, UL_coords, UR_coords, -1, -1); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 


}
Light *parse_point_light(char *line) {
	float param[7];
	fill_param_float(param, 7);
	Vector3f xyz = Vector3f(param[0], param[1], param[2]);
	Vector3f RGB = Vector3f(param[3], param[4], param[5]);
	float fall_off = param[6];
	return new PointLight(xyz, RGB, fall_off);
	
}
Light *parse_directional_light(char *line) {
	float param[6];
	fill_param_float(param, 6);
	Vector3f xyz = Vector3f(param[0], param[1], param[2]);
	Vector3f RGB = Vector3f(param[3], param[4], param[5]);
	return new DirectionalLight(xyz, RGB);
}
Vector3f parse_ambient_light(char *line) {
	float param[6];
	fill_param_float(param, 3);
	return Vector3f(param[0], param[1], param[2]);
}
Sphere *parse_sphere(char *line, BRDF *current_mat) {
	int param[4];
	fill_param(param, 4);
	Vector3f sphere_center_coords = Vector3f(param[0], param[1], param[2]);

	return new Sphere(sphere_center_coords, param[4], current_mat);

}

Triangle *parseTriangle(char *line, BRDF *current_mat) {
	int param[9];
	fill_param(param, 9);
	Vector3f vert_1 = Vector3f(param[0], param[1], param[2]);
	Vector3f vert_2 = Vector3f(param[3], param[4], param[5]);
	Vector3f vert_3 = Vector3f(param[6], param[7], param[8]);
	return new Triangle(vert_1, vert_2, vert_3, current_mat);
}

void parseTransformation(char *line, Transformation *trans, enum TRANSFORMATION t) {
	int param[3];
	fill_param(param, 3);
	Vector3f t_vector = Vector3f(param[0], param[1], param[2]);
	switch (t){

		case ROTATION:
			trans->rotate(t_vector);
		case TRANSLATION:
			trans->translate(t_vector);
		case SCALE:
			trans->scale(t_vector);
		default:
			printf("How did this even happen?\n");
			exit(0);
	}
}

void parseBRDF(char *line, BRDF *curr_b) {
	float param[13];
	fill_param_float(param, 13);
	Vector3f ambientRGB = Vector3f(param[0], param[1], param[2]);
	Vector3f diffuseRGB = Vector3f(param[3], param[4], param[5]);
	Vector3f specularRGB = Vector3f(param[6], param[7], param[8]);
	float specularity_coefficent = param[9];
	Vector3f reflectiveRGB = Vector3f(param[10], param[11], param[12]);
	curr_b->k_a = ambientRGB;
	curr_b->k_d = diffuseRGB;
	curr_b->k_s = specularRGB;
	curr_b->k_r = reflectiveRGB;
	curr_b->k_sp = specularity_coefficent;
}

/*
INPUT: A string file name and a ptr to Scene object. 

OUTPUT: A filled scene object with fields reflecting information in the text file. Can error if the text file is misformed. 

Iterate line by line through the input file. For each line, tokenize using strtok() and use the first line to determine how to parse the rest of the 
input. Unfortunately there is no easy way to make this a switch since string equality is not a simple matter of checking pointer equality. 

*/
void parse_loop(char *file_name, struct parser_struct *parser_fill) {
	FILE *fp = fopen(file_name, "r"); //File pointer to the file we want to read. 
	if (!fp) {//file could not be read, exit. 
		printf("%s could not be read, exiting. ", file_name);
		exit(0);
	}

	std::vector<Shape*> *objects = new std::vector<Shape*>();
	std::vector<Light*> *light_list = new std::vector<Light*>();
	Transformation current_transform = Transformation();
	BRDF *current_BRDF = new BRDF();
	char line[200];
	while (fgets(line, 200, fp) != NULL) { //Get next line with limit of 200 character lines until we hit the EOF. 


		char *token;
		token = strtok(line, " ");
		if (strcmp(token, "cam")) {
			parser_fill->camera = parse_camera(line);
		}
		if (strcmp(token, "sph")) {
			Sphere *new_sphere = parse_sphere(line, current_BRDF);
			new_sphere->transformation = current_transform;
			objects->push_back(new_sphere);
		}
		if (strcmp(token, "tri")) {
			Triangle *new_triangle = parseTriangle(line, current_BRDF);
			new_triangle->transformation = current_transform;
			objects->push_back(new_triangle);
		}
		if(strcmp(token, "obj")) {
			//Handle this somehow
		}

		if(strcmp(token, "ltp")) {
			light_list->push_back(parse_point_light(line));
		}

		if (strcmp(token, "ltd")) {
			light_list->push_back(parse_directional_light(line));
		}

		if (strcmp(token, "lta")) {
			parser_fill->ambient_light = parse_ambient_light(line);
		}

		if (strcmp(token, "mat")) {
			parseBRDF(line, current_BRDF);
		}

		if (strcmp(token, "xft")) {
			parseTransformation(line, &current_transform, TRANSLATION);
		}
		if (strcmp(token, "xfr")) {
			parseTransformation(line, &current_transform, ROTATION);
		}
		if (strcmp(token, "xfs")) {
			parseTransformation(line, &current_transform, SCALE);
		}
		if (strcmp(token, "xfz")) { //Flush the transformation by setting to default. 
			current_transform = Transformation();
		}

		else {
			printf("Unrecognized item: %s in bagging area! Please wait for assistance from technician!\n", token);
			exit(0);
		}



	}
	parser_fill->obj_list = AggregatePrimitive(*objects);
	parser_fill->light_list = light_list;


	char *output_string = (char *)malloc(50); //Maintain this string on stack so it doesn't get clobbered when parse_loop exits. 
	strcpy(output_string, file_name);
	strcat(output_string, ".png");
	parser_fill->file_name = output_string;
}

