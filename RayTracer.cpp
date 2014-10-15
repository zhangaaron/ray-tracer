#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>

#define PI 3.14159265  // Should be used from mathlibs
using namespace std;
using namespace Eigen;

class Scene {
	public: 
		Scene(Vector3d pEye_Vector, MatrixXd pImage_plane, Vector2i pPixel_dimensions) {

		}
	private:
		Vector3d eye_Vector;
		MatrixXd image_plane;
		Vector2i pixel_dimensions;
};



int main(int argc, char *argv[]) {
  

  return 0;
}


