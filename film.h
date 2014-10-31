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

