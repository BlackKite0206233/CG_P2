#pragma once
#include <vector>
#include "Mat.h"

using namespace std;

class Mat4D: public Mat {
public:
	Mat4D() : Mat(4) {};
	Mat4D(const Mat&);

	void Perspective(double, double, double, double);
	void Rotate(double, double, double, double);
	void Translate(double, double, double);
};

