#pragma once
#include <vector>
#include "Vec3D.h"
#include "Vec4D.h"

using namespace std;

class Mat4D {
public:
	Mat4D();
	double Det();
	Mat4D Inverted();
	void SetToIdentity();
	void Perspective(double, double, double, double);
	void Rotate(double, double, double, double);
	void Translate(double, double, double);

	friend Mat4D operator+(const Mat4D&, const Mat4D&);
	friend Mat4D operator-(const Mat4D&, const Mat4D&);
	friend Mat4D operator*(const Mat4D&, const Mat4D&);
	friend Vec4D operator*(const Mat4D&, const Vec4D&);
	friend Vec4D operator*(const Vec4D&, const Mat4D&);
	friend Mat4D operator*(const Mat4D&, double);
	friend Mat4D operator*(double, const Mat4D&);
	friend Mat4D operator/(const Mat4D&, double);
	friend Mat4D operator/(double, const Mat4D&);

	Mat4D operator+=(const Mat4D&);
	Mat4D operator-=(const Mat4D&);
	Mat4D operator*=(const Mat4D&);
	Vec4D operator*=(const Vec4D&);
	Mat4D operator*=(double);
	Mat4D operator/=(double);
private:
	vector<vector<double>> mat;
};

