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
		char *token;
		token = strtok(line, " ");
		if (strcmp(token, "cam")) {
			scene_to_fill->parseCamera = parseCamera();
		}
		if (strcmp(token, "sph")) {
			parseSphere();
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

		}

		if (strcmp(token, "xft")) {

		}
		if (strcmp(token, "xfr")) {
			
		}
		if (strcmp(token, "xfs")) {
			
		}
		if (strcmp(token, "xfs")) {
			current_transform = Transformation();
		}

		else {
			printf("Unrecognized item: %s in bagging area! Please wait for assistance from technician!\n", token);
			exit(0);
		}



	}

}

Camera parseCamera(char *line) {
	int param[15];
	for (int i = 0; i < 15, i++) {
		char *token = strtok(NULL, " ");
		if (token == NULL) {
			printf("Malformed camera string in object file, exiting. \n");
		}
		param[i] = atoi(token);
	}
	Vector3f eye_coords = Vector3f(param[0], param[1], param[2]);
	Vector3f LL_coords = Vector3f(param[3], param[4], param[5]);
	Vector3f LR_coords = Vector3f(param[6], param[7], param[8]);
	Vector3f UL_coords = Vector3f(param[9], param[10], param[11]);
	Vector3f UR_coords = Vector3f(param[12], param[13], param[14]);
	return Camera(eye_coords, LL_coords, LR_coords, UL_coords, UR_coords, -1, -1); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 


}

Sphere parseSphere(char *line, Transformation *transformation) {
	int param[4];
	for (int i = 0; i < 15, i++) {
		char *token = strtok(NULL, " ");
		if (token == NULL) {
			printf("Malformed sphere string in object file, exiting. \n");
		}
		param[i] = atoi(token);
	}
	Vector3f sphere_center_coords = Vector3f(param[0], param[1], param[2]);

	return Sphere(sphere_center_coords, param[4], current_mat); //Parser doesn't know what output_x and y is so put -1 to remember to change later. 

}