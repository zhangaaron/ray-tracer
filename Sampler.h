#pragma once // Include guard
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include "math.h"

using namespace std;
using namespace Eigen;

class Sampler {
	public: 
		Sampler(int X_size, int Y_size) {
			this->X_size = X_size; 
			this->Y_size = Y_size;
		}
	private:
		int X_size;
		int Y_size;
		int sample_iteration;


};



