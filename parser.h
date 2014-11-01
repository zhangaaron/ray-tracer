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

 This file can be used to parse input files given to our ray tracer and return an AggregatePrimitive Object as well as the necessary output dimensions and file name. 
 */
enum TRANSFORMATION {
	TRANSLATION,
	ROTATION, 
	SCALE
};

enum LIGHT {
	POINT,
	DIFFUSE,
	AMBIENT
};
	


void parse(char *file_name, Scene *scene_to_fill) {

}

void parseLoop(char *file_name) {
	File *fp = fopen(file_name); //File pointer to the file we want to read. 
	if (!fp) {//file could not be read, exit. 
		printf("%s could not be read, exiting. ", file_name);
		exit(0);
	}
	char line[200];
	while (fscanf(line, 200, fp) != NULL) {

		Transformation current_transform = Transformation();
		BRDF *current_BRDF = new BRDF();
		char *token;
		token = strtok(line, " ");
		if (strcmp(token, "cam")) {
			scene_to_fill->parseCamera = parseCamera();
		}
		if (strcmp(token, "sph")) {
			Sphere *new_sphere = parseSphere();
		}
		if (strcmp(token, "tri")) {

		}
		if(strcmp(token, "obj")) {

		}

		if(strcmp(token, "ltp")) {

		}

		if (strcmp(token, "ltd")) {

		}

		if (strcmp(token, "lta")) {

		}

		if (strcmp(token, "mat")) {
			parseBRDF(line, BRDF *curr_b);
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

}

//For a space seperated line consisting of numbers, convert every input into integer and put into fill array. 
void fill_param(int* fill, int size) {
		for (int i = 0; i < size, i++) {
			char *token = strtok(NULL, " ");
			if (token == NULL) {
				printf("Malformed string in object file, exiting. \n");
			}
			fill[i] = atoi(token);
		}
}

Camera parseCamera(char *line) {
	int param[15];
	fill_param(param, 15);
	Vector3f eye_coords = Vector3f(param[0], param[1], param[2]);
	Vector3f LL_coords = Vector3f(param[3], param[4], param[5]);
	Vector3f LR_coords = Vector3f(param[6], param[7], param[8]);
	Vector3f UL_coords = Vector3f(param[9], param[10], param[11]);
	Vector3f UR_coords = Vector3f(param[12], param[13], param[14]);
	return Camera(eye_coords, LL_coords, LR_coords, UL_coords, UR_coords, -1, -1); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 


}

Sphere *parseSphere(char *line, Transformation *transformation) {
	int param[4];
	fill_param(param, 4);
	Vector3f sphere_center_coords = Vector3f(param[0], param[1], param[2]);

	return new Sphere(sphere_center_coords, param[4], current_mat); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 

}

void parseTransformation(char *line, Transformation *trans, enum TRANSFORMATION t) {
	int param[3];
	fill_param(param, 3);
	switch (t){
		Vector3f transformation_vector = Vector3f(param[0], param[1], param[2]);
		case ROTATION:
			trans->rotate(transformation_vector);
		case TRANSLATION:
			trans->translate(transformation_vector);
		case SCALE:
			trans->scale(transformation_vector);
		default:
			printf("How did this even happen?\n");
			exit(0);
	}
}
