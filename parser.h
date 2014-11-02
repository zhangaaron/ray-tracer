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
#define TRANSLATION 0
#define ROTATION 1
#define SCALE 2

//This gets filled out by the parse_loop
struct parser_struct {
	Camera camera;
	AggregatePrimitive obj_list;
	std::vector<Light*> *light_list;
	Vector3f ambient_light;
	char * file_name;
};

//For a space seperated line consisting of numbers, convert every input into integer and put into fill array. 
char * tokenize_line(char *line, int *int_array, float *float_array) {
		char *token = strtok(line, " "); 
		for (int i = 0; i < 15; i++) {
			char *value = strtok(NULL, " ");
			if (value == NULL) {
				break;
			}
			int_array[i] = atoi(value);
			float_array[i] = atof(value);
		}
		return token;
}

//For a space seperated line consisting of numbers, convert every input into float and put into fill array. 
void fill_param_float(float* fill, int size, char* line) {
		for (int i = 0; i < size; i++) {
			char *token = strtok(NULL, " ");
			if (token == NULL) {
				printf("Malformed string in line: %s, exiting. \n", line);
			}
			fill[i] = atof(token);
		}
}

Camera parse_camera(int *param) {
	Vector3f eye_coords = Vector3f(param[0], param[1], param[2]);
	Vector3f LL_coords = Vector3f(param[3], param[4], param[5]);
	Vector3f LR_coords = Vector3f(param[6], param[7], param[8]);
	Vector3f UL_coords = Vector3f(param[9], param[10], param[11]);
	Vector3f UR_coords = Vector3f(param[12], param[13], param[14]);
	return Camera(eye_coords, LL_coords, LR_coords, UL_coords, UR_coords, 1000, 1000); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 


}
Light *parse_point_light(float *param) {

	Vector3f xyz = Vector3f(param[0], param[1], param[2]);
	Vector3f RGB = Vector3f(param[3], param[4], param[5]);
	float fall_off = param[6];
	return new PointLight(xyz, RGB, fall_off);
	
}
Light *parse_directional_light(float *param) {

	Vector3f xyz = Vector3f(param[0], param[1], param[2]);
	Vector3f RGB = Vector3f(param[3], param[4], param[5]);
	return new DirectionalLight(xyz, RGB);
}
Vector3f parse_ambient_light(float *param) {

	return Vector3f(param[0], param[1], param[2]);
}
Sphere *parse_sphere(int *param, BRDF *current_mat) {
	Vector3f sphere_center_coords = Vector3f(param[0], param[1], param[2]);

	return new Sphere(sphere_center_coords, param[3], current_mat);

}

Triangle *parseTriangle(int *param, BRDF *current_mat) {
	Vector3f vert_1 = Vector3f(param[0], param[1], param[2]);
	Vector3f vert_2 = Vector3f(param[3], param[4], param[5]);
	Vector3f vert_3 = Vector3f(param[6], param[7], param[8]);
	return new Triangle(vert_1, vert_2, vert_3, current_mat);
}

void parseTransformation(int *param, Transformation *trans, int t) {
	Vector3f t_vector = Vector3f(param[0], param[1], param[2]);
	switch (t){

		case ROTATION:
			trans->rotate(t_vector);
			break;
		case TRANSLATION:
			trans->translate(t_vector);
			break;
		case SCALE:
			trans->scale(t_vector);
			break;
		default:
			printf("How did this even happen? %d \n", t);
			exit(0);
	}
}

void parseBRDF(float *param, BRDF *curr_b) {
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
		char *token = "A";
		int integer_tokens[15];
		float float_tokens[15];
		token = tokenize_line(line, integer_tokens, float_tokens);
		if (!strcmp(token, "cam")) {
			parser_fill->camera = parse_camera(integer_tokens);
		}
		if (!strcmp(token, "sph")) {
			Sphere *new_sphere = parse_sphere(integer_tokens, current_BRDF);
			new_sphere->transformation = current_transform;
			objects->push_back(new_sphere);
		}
		if (!strcmp(token, "tri")) {
			Triangle *new_triangle = parseTriangle(integer_tokens, current_BRDF);
			new_triangle->transformation = current_transform;
			objects->push_back(new_triangle);
		}
		if(!strcmp(token, "obj")) {
			//Handle this somehow
		}

		if(!strcmp(token, "ltp")) {
			light_list->push_back(parse_point_light(float_tokens));
		}

		if (!strcmp(token, "ltd")) {
			light_list->push_back(parse_directional_light(float_tokens));
		}

		if (!strcmp(token, "lta")) {
			parser_fill->ambient_light = parse_ambient_light(float_tokens);
		}

		if (!strcmp(token, "mat")) {
			parseBRDF(float_tokens, current_BRDF);
		}

		if (!strcmp(token, "xft")) {
			parseTransformation(integer_tokens, &current_transform, TRANSLATION);
		}
		if (!strcmp(token, "xfr")) {
			parseTransformation(integer_tokens, &current_transform, ROTATION);
		}
		if (!strcmp(token, "xfs")) {
			parseTransformation(integer_tokens, &current_transform, SCALE);
		}
		if (!strcmp(token, "xfz")) { //Flush the transformation by setting to default. 
			current_transform = Transformation();
		}

		// else {
		// 	printf("Unrecognized item: %s in bagging area! Please wait for assistance from technician!\n", token);
		// 	exit(0);
		// }



	}
	parser_fill->obj_list = AggregatePrimitive(*objects);
	parser_fill->light_list = light_list;


	char *output_string = (char *)malloc(50); //Maintain this string on stack so it doesn't get clobbered when parse_loop exits. 
	strcpy(output_string, file_name);
	strcat(output_string, ".png");
	parser_fill->file_name = output_string;
}

