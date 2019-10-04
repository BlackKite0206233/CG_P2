#include "Mat4D.h"

Mat4D::Mat4D() {
	mat = vector<vector<double>>(4, vector<double>(4, 0));
}

double Mat4D::Det() {

}

Mat4D Mat4D::Inverted() {

}

void Mat4D::SetToIdentity() {
	mat = vector<vector<double>>(4, vector<double>(4, 0));
	for (int i = 0; i < 4; i++) {
		mat[i][i] = 1;
	}
}

void Mat4D::Perspective(double fov, double aspecRatio, double near, double far) {

}

void Mat4D::Rotate(double angle, double x, double y, double z) {

}

void Mat4D::Translate(double x, double y, double z) {

}

Mat4D operator+(const Mat4D& m1, const Mat4D& m2) {
	Mat4D res;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res.mat[i][j] = m1.mat[i][j] + m2.mat[i][j];
		}
	}
	return res;
}

Mat4D operator-(const Mat4D& m1, const Mat4D& m2) {
	Mat4D res;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res.mat[i][j] = m1.mat[i][j] - m2.mat[i][j];
		}
	}
	return res;
}

Mat4D operator*(const Mat4D& m1, const Mat4D& m2) {
	Mat4D res;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				res.mat[i][j] += m1.mat[i][k] * m2.mat[k][j];
			}
		}
	}
	return res;
}

Vec4D operator*(const Mat4D& m, const Vec4D& v) {
	Vec4D res;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res.vec[i] += v.vec[i] * m.mat[i][j];
		}
	}
	return res;
}