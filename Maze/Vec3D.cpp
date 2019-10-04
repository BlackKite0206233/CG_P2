#include "Vec3D.h"

double Vec3D::DotProduct(const Vec3D& v1, const Vec3D& v2) {
	double res = 0;
	for (int i = 0; i < 3; i++) {
		res += v1.vec[i] * v2.vec[i];
	}
	return res;
}

Vec3D Vec3D::CrossProduct(const Vec3D& v1, const Vec3D& v2) {
	Vec3D res;
	res.vec[0] = v1.vec[1] * v2.vec[2] - v1.vec[2] * v2.vec[1];
	res.vec[1] = v1.vec[2] * v2.vec[0] - v1.vec[0] * v2.vec[2];
	res.vec[2] = v1.vec[0] * v2.vec[1] - v1.vec[1] * v2.vec[0];
	return res;
}

Vec3D Vec3D::Normal(const Vec3D& v1, const Vec3D& v2) {
	Vec3D res = CrossProduct(v1, v2);
	return res.Normalized();
}

Vec3D Vec3D::Normal(const Vec3D& v1, const Vec3D& v2, const Vec3D& v3) {
	Vec3D res = CrossProduct(v2 - v1, v3 - v1);
	return res.Normalized();
}

Vec3D::Vec3D(double x, double y, double z) : Vec(3) {
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
}

Vec3D::Vec3D(const Vec& v): Vec(3) {
	for (int i = 0; i < 3 && i < v.size; i++) {
		vec[i] = v.vec[i];
	}
}

void Vec3D::SetX(double n) {
	vec[0] = n;
}

void Vec3D::SetY(double n) {
	vec[1] = n;
}

void Vec3D::SetZ(double n) {
	vec[2] = n;
}

double Vec3D::x() {
	return vec[0];
}

double Vec3D::y() {
	return vec[1];
}

double Vec3D::z() {
	return vec[2];
}
