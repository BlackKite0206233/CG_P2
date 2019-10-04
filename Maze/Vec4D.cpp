#include "Vec4D.h"

double Vec4D::DotProduct(const Vec4D& v1, const Vec4D& v2) {
	double res = 0;
	for (int i = 0; i < 4; i++) {
		res += v1.vec[i] * v2.vec[i];
	}
	return res;
}

Vec4D::Vec4D(double x, double y, double z, double w): Vec(4) {
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	vec[3] = w;
}

void Vec4D::SetX(double n) {
	vec[0] = n;
}

void Vec4D::SetY(double n) {
	vec[1] = n;
}

void Vec4D::SetZ(double n) {
	vec[2] = n;
}

void Vec4D::SetW(double n) {
	vec[3] = n;
}

double Vec4D::x() {
	return vec[0];
}

double Vec4D::y() {
	return vec[1];
}

double Vec4D::z() {
	return vec[2];
}

double Vec4D::w() {
	return vec[3];
}