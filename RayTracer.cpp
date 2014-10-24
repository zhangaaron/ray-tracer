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
	public:
		Vector3f pos;
		Vector3f dir;
		float t_min, t_max;
		Ray(float min_t, float max_t);
		Ray(Vector3f pos_input, Vector3f dir_input, float min_t, float max_t);
	private:
};

Ray::Ray(float min_t, float max_t){
	t_min = min_t;
	t_max = max_t;
};

Ray::Ray(Vector3f pos_input, Vector3f dir_input, float min_t, float max_t){
	pos = pos_input;
	dir = dir_input;
	t_min = min_t;
	t_max = max_t;
};


class Sampler {
	public:
		int x_dim, y_dim, current_x, current_y;
		bool done;

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

	public:
		int x, y;
		MatrixXf output_image_r;
		MatrixXf output_image_g;
		MatrixXf output_image_b;

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
	public:
		Vector3f camera_coord;
		Vector3f ll;
		Vector3f lr;
		Vector3f ul;
		Vector3f ur;
		int output_x;
		int output_y;

		Camera(Vector3f coord, Vector3f lleft, Vector3f lright, Vector3f ulleft, Vector3f uright, int x, int y);
		void generateRay(Sample& sample, Ray* ray);
	private:
};

Camera::Camera(Vector3f coord, Vector3f lleft, Vector3f lright, Vector3f ulleft, Vector3f uright, int x, int y){
	camera_coord = coord;
	ll = lleft;
	lr = lright;
	ul = ulleft;
	ur = uright;
	output_x = x;
	output_y = y;
};

void Camera::generateRay(Sample& sample, Ray* ray){
	float u = sample.x / ((float)output_x);
	float v = sample.y / ((float)output_y);
	//printf("u: %d %f\tv: %d %f\n", sample.x, u, sample.y, v);
	Vector3f P = u*(v*ll + (1-v)* ul) + (1-u)*(v*lr + (1-v) * ur);
	ray->pos = camera_coord;
	ray->dir = P-camera_coord;
};



class Scene {
	public:
		Sampler mySampler;
		Film myFilm;
		Camera myCamera;

		Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y);
		void render();
	private:
};

Scene::Scene(Vector3f cam_coord, Vector3f ll, Vector3f lr, Vector3f ul, Vector3f ur, int output_x, int output_y) : mySampler(output_x, output_y), myFilm(output_x, output_y), myCamera(cam_coord, ll, lr, ul, ur, output_x, output_y){
};

void Scene::render() {
	Sample sample (0,0);
	Ray ray(0, 1);
	while (mySampler.getSample(&sample)) {
		myCamera.generateRay(sample, &ray);
		printf("Ray at X: %d Y: %d\n", sample.x, sample.y);
		printf("Pos: (%f, %f, %f)\tDirection: (%f, %f, %f)\n\n\n", ray.pos[0], ray.pos[1], ray.pos[2], ray.dir[0], ray.dir[1], ray.dir[2]);
	}
};

int main(int argc, char *argv[]) {
	Vector3f cam_coord(0, 0, 0);
	Vector3f ll(50, 100, -50);
	Vector3f lr(50, -100, -50);
	Vector3f ul(50, 100, 50);
	Vector3f ur(50, -100, 50);
	Scene myScene(cam_coord, ll, lr, ul, ur, 200, 100);
	myScene.render();
	return 0;
}