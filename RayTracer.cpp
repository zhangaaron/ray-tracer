#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>
#include "Sampler.h"

using namespace std;
using namespace Eigen;

class Scene {
	public: 
		Scene(Vector3d pEye_Vector, MatrixXd pImage_plane, Vector2i pPixel_dimensions) {
			eye_Vector = pEye_Vector;
			image_plane = pImage_plane;
			pixel_dimensions = pPixel_dimensions;
			mySampler = NULL;
		}
	private:
		Vector3d eye_Vector;
		MatrixXd image_plane;
		Vector2i pixel_dimensions;
		Sampler mySampler;
		// Camera myCamera;
		// RayTracer myRayTracer;
		// Film myFilm;

};



int main(int argc, char *argv[]) {
  MatrixXd m = MatrixXd::Random(3,3);
  return 0;
}


