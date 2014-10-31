#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <Eigen/Dense>
#include <time.h>
#include <math.h>

using namespace std;
using namespace Eigen;

class Transformation {
	public:
		Transform<float, 3, Affine, DontAlign> matrix_trans;
		Transform<float, 3, Affine, DontAlign> matrix_inv_transp; //Inverse transpose of the transformation matrix, used for transforming the normal vectors. 
		Transformation() { //Initializes a transformation that does nothing. 
			matrix_trans = AngleAxisf(0, Vector3f::UnitX());
			matrix_inv_transp = AngleAxisf(0, Vector3f::UnitX()); 
		}
		void set_inverse_transpose() {
			matrix_inv_transp = matrix_trans.inverse().matrix().transpose();
		}
		void translate(Vector3f xyz) {
			matrix_trans = matrix_trans * Translation<float, 3>(xyz);
			set_inverse_transpose();
		}
		void rotate(Vector3f xyz) {
			matrix_trans = matrix_trans * ( AngleAxisf(xyz[0], Vector3f::UnitX()) * 
										 AngleAxisf(xyz[1], Vector3f::UnitY()) *
										  AngleAxisf(xyz[2], Vector3f::UnitZ()));
			set_inverse_transpose();
		}
		 void scale(Vector3f xyz) {
		 	matrix_trans = matrix_trans * Scaling(xyz);
		 	set_inverse_transpose();
		 }

		void print() {
			cout << "Rotation matrix: \n" << matrix_trans.rotation() << "\n";
			cout << "Translation: \n " << matrix_trans.translation() << "\n"; 
		}
		

	private: 
};
