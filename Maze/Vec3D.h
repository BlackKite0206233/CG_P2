#pragma once
#include "Vec.h"

class Vec3D: public Vec {
public:
	static double DotProduct(const Vec3D&, const Vec3D&);
	static Vec3D CrossProduct(const Vec3D&, const Vec3D&);
	static Vec3D Normal(const Vec3D&, const Vec3D&);
	static Vec3D Normal(const Vec3D&, const Vec3D&, const Vec3D&);

	Vec3D() : Vec(3) {};
	Vec3D(double, double, double);
	Vec3D(const Vec&);

	void SetX(double);
	void SetY(double);
	void SetZ(double);

	double x();
	double y();
	double z();
};

