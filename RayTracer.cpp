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

class Sample {
	public:
		int x, y;
		Sample(int x_in, int y_in);
};

Sample::Sample(int x_in, int y_in){
	x = x_in;
	y = y_in;
};

class Ray {
	Vector2f pos;
	Vector3f dir;
	float t_min, t_max;
	public:
		Ray(Vector2f pos_input, Vector3f dir_input, float min_t, float max_t);
	private:
};

Ray::Ray(Vector2f pos_input, Vector3f dir_input, float min_t, float max_t){
	pos = pos_input;
	dir = dir_input;
	t_min = min_t;
	t_max = max_t;
};


class Sampler {
	int x_dim, y_dim, current_x, current_y;
	bool done;
	public:
		Sampler(int x, int y);
		bool getSample(Sample* sample);
	private:
};

Sampler::Sampler(int x, int y){
	x_dim = x;
	y_dim = y;
	current_x = -1; //Off by 1 error hack
	current_y = 0;
	done = false;
};

bool Sampler::getSample(Sample* sample){
	if (done == true){
		return false;
	}
	else if (current_x < x_dim){
		current_x += 1;
	}
	else if (current_x == x_dim && current_y < y_dim){
		current_y = current_y + 1;
		current_x = 0;
	}
	else if (current_x == x_dim && current_y == y_dim){
		done = true;
	}
	sample->x = current_x;
	sample->y = current_y;
	return true;
};

class Film {
	int x, y;
	MatrixXf output_image_r;
	MatrixXf output_image_g;
	MatrixXf output_image_b;
	public:
		Film(int output_x, int output_y);
		void commit(Sample& sample, Array3f color);
		void writeImage();
	private:
};

Film::Film(int output_x, int output_y){
	x = output_x;
	y = output_y;
	MatrixXf output_image_r(output_x, output_y);
	MatrixXf output_image_g(output_x, output_y);
	MatrixXf output_image_b(output_x, output_y);
}

void Film::commit(Sample& sample, Array3f color){
	output_image_r(sample.x, sample.y) = color[0];
	output_image_g(sample.x, sample.y) = color[1];
	output_image_b(sample.x, sample.y) = color[2];
}

void writeImage(){
	//Figure out how to write to image
}

class Camera {
	Vector3f camera_coord;
	public:
		Camera(Vector3f coord);
		void generateRay(Sample& sample, Ray* ray);
	private:
};

Camera::Camera(Vector3f coord){
	camera_coord = coord;
};

class Scene {
	Sampler mySampler;
	Film myFilm;
	public:
		Scene(int output_x, int output_y);
		void render();
	private:
};

Scene::Scene(int output_x, int output_y) : mySampler(output_x, output_y), myFilm(output_x, output_y){
};

void Scene::render() {
	Sample sample (0,0);
	while (mySampler.getSample(&sample)) {
		printf("X: %d Y: %d\n", sample.x, sample.y);
	}
};

int main(int argc, char *argv[]) {
	Scene myScene(100, 100);
	myScene.render();
	return 0;
}