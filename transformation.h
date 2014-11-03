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
		Transform<float, 3, Affine, DontAlign> matrix_transformation_point;
		Transform<float, 3, Affine, DontAlign> matrix_transformation_ray; //Inverse transpose of the transformation matrix, used for transforming the normal vectors. 
		Transformation() { //Initializes a transformation that does nothing. 
			matrix_transformation_point = AngleAxisf(0, Vector3f::UnitX());
			matrix_transformation_ray = AngleAxisf(0, Vector3f::UnitX()); 
		}
		void translate(Vector3f xyz) {
			matrix_transformation_point = matrix_transformation_point * Translation<float, 3>(xyz);
		}
		void rotate(Vector3f xyz) {
			matrix_transformation_ray = matrix_transformation_ray * ( AngleAxisf(xyz[0], Vector3f::UnitX()) * 
										 AngleAxisf(xyz[1], Vector3f::UnitY()) *
										  AngleAxisf(xyz[2], Vector3f::UnitZ()));
			matrix_transformation_point = matrix_transformation_point * ( AngleAxisf(xyz[0], Vector3f::UnitX()) * 
							 AngleAxisf(xyz[1], Vector3f::UnitY()) *
							  AngleAxisf(xyz[2], Vector3f::UnitZ()));

		}
		 void scale(Vector3f xyz) {
			 	matrix_transformation_point = matrix_transformation_point * Scaling(xyz);
			 	matrix_transformation_ray = matrix_transformation_ray * Scaling(xyz);
		 }

		void print() {
			cout << "Rotation matrix: \n" << matrix_transformation_point.rotation() << "\n";
			cout << "Translation: \n " << matrix_transformation_point.translation() << "\n"; 
		}

		void flush() {
			matrix_transformation_point = AngleAxisf(0, Vector3f::UnitX());
			matrix_transformation_ray = AngleAxisf(0, Vector3f::UnitX()); 
		}
		

	private: 
};
