#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>

class Sampler {
	public:
		int x_max, y_max;
		int x, y;
		Sampler() {}
		Sampler(int x_max, int y_max){
			this->x_max = x_max;
			this->y_max= y_max;
			x = 0;
			y = 0;
		};
		bool hasNext();
		void next(int *XYCoords);

	private:
};

bool Sampler::hasNext(){
	return (x < x_max && y < y_max);
}
void Sampler::next(int* XYCoords){
	XYCoords[0] = x;
	XYCoords[1] = y;
	x = (x + 1) % x_max;
	if (x == 0)	y++;
	
};
