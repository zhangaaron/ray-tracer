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

class Scene {
	Sampler mySampler;
	public:
		Scene(int output_x, int output_x);
		void render();
	private:
};

Scene::Scene() : mySampler(int output_x, int output_y){
};

void Scene::render() {
	Sample sample (0,0);
	printf("Scene render called\n");
	while (mySampler.getSample(&sample)) {
		printf("X: %d Y: %d\n", sample.x, sample.y);
	}
};

int main(int argc, char *argv[]) {
	Scene myScene;
	myScene.render();
	return 0;
}