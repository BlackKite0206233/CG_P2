#pragma once
#include "Vec.h"

class Vec4D: public Vec {
public:
	static double DotProduct(const Vec4D&, const Vec4D&);

	Vec4D() : Vec(4) {};
	Vec4D(double, double, double, double);
	Vec4D(const Vec&);

	void SetX(double);
	void SetY(double);
	void SetZ(double);
	void SetW(double);

	double x();
	double y();
	double z();
	double w();

};

