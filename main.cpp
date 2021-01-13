#include <stdio.h>
#include <iostream>
#include <cmath>
#include <limits>
#include <array>
using namespace std;
#include <stdlib.h>
#include <typeinfo>
#include "perceptron.hpp"


class functions{
	public:
	static void* malloc(int const size){
		return std::malloc(size);
	}
	static void free(void* p){
		std::free(p);
	}
	static double log(double const x){
		return std::log(x);
	}
	static double log2(double const x){
		return std::log2(x);
	}
	static double exp(double const x){
		return std::exp(x);
	}
	static double sqrt(double const x){
		return std::sqrt(x);
	}
	static bool isnan(double const x){
		return std::isnan(x);
	}
	//TODO: we need a rand_uniform and a log functions (log == ln)
	static double rand_uniform(void){
		return static_cast<double>(rand())/static_cast<double>(RAND_MAX);
	}
	static double random(void){
		return (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));
	}
	template<class T>
	static int round(T const x){
		return std::round(x);
	}
	template<class T>
	static int floor(T const x){
		return std::floor(x);
	}
	static double activation(double input){
		return 1.0 / (1 + exp(-input));
	}
	static double derivative(double const x){
		return x * (1 - x);
	}

};
template<int layer_count>
void parrot(int repeat){
	int layer_size[layer_count] = {2, 3, 1};

	MultiLayerPerceptron<layer_count, 600000, functions>* mlp = new MultiLayerPerceptron<layer_count, 600000, functions>(layer_size, 0.001);
	double err = 1;
	double min_err = 1;
	int i = 0;
	while(err > 0.001){
		i += 1;
		int f_int[2] = {rand()%2, rand()%2};
		//int f_int[2] = {0, 1};
		double f_double[2] = {static_cast<double>(f_int[0]), static_cast<double>(f_int[1])};
		double output[2];
		int idx = static_cast<int>(f_int[0]^f_int[1]);
		double target[2];
		target[idx] = 1;
		target[(idx+1)%2] = 0;
		mlp->feed_forward(f_double, output);
		err = sqrt(0.5*mlp->backpropagate(target));
		if(err < min_err)
			min_err = err;
		if(i%1000 == 0)
		cout << "ERROR: " << err << " ---- " << min_err << endl;
		
	}
	cout << "Last RMSE: " << err << endl;
}

int main(){
	parrot<3>(60000);
	return 0;
}
