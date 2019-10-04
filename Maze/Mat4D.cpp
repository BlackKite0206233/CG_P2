#include "Mat4D.h"
#include "Vec3D.h"

#define M_PI 3.1415926

Mat4D::Mat4D(const Mat& m): Mat(4) {
	for (int i = 0; i < 4 && i < m.size; i++) {
		for (int j = 0; j < 4 && j < m.size; j++) {
			mat[i][j] = m.mat[i][j];
		}
	}
}

void Mat4D::Perspective(double fov, double aspectRatio, double near, double far) {
	double halfFov = fov / 2;
	if (near == far || aspectRatio == 0 || halfFov == 0 || halfFov == 180 || halfFov == -180) {
		return;
	}
	Mat4D m;
	double rad = halfFov * M_PI / 180;
	double cot = cos(rad) / sin(rad);
	double clip = far - near;
	m.mat[0][0] = cot / aspectRatio;
	m.mat[1][0] = 0.0f;
	m.mat[2][0] = 0.0f;
	m.mat[3][0] = 0.0f;
	m.mat[0][1] = 0.0f;
	m.mat[1][1] = cot;
	m.mat[2][1] = 0.0f;
	m.mat[3][1] = 0.0f;
	m.mat[0][2] = 0.0f;
	m.mat[1][2] = 0.0f;
	m.mat[2][2] = -(near + far) / clip;
	m.mat[3][2] = -(2.0f * near * far) / clip;
	m.mat[0][3] = 0.0f;
	m.mat[1][3] = 0.0f;
	m.mat[2][3] = -1.0f;
	m.mat[3][3] = 0.0f;
	(*this) *= m;
}

void Mat4D::Rotate(double angle, double x, double y, double z) {
	if (angle == 0) {
		return;
	}
	double c, s;
	if (angle == 90 || angle == -270) {
		s = 1;
		c = 0;
	}
	else if (angle == -90 || angle == 270) {
		s = -1;
		c = 0;
	}
	else if (angle == 180 || angle == -180) {
		s = 0;
		c = -1;
	}
	else {
		double rad = angle * M_PI / 180;
		c = cos(rad);
		s = sin(rad);
	}
	if (x == 0) {
		if (y == 0) {
			if (z != 0) {
				if (z < 0) {
					s = -s;
				}
				double tmp;
				mat[0][0] = (tmp = mat[0][0]) * c + mat[1][0] * s;
				mat[1][0] = mat[1][0] * c - tmp * s;
				mat[0][1] = (tmp = mat[0][1]) * c + mat[1][1] * s;
				mat[1][1] = mat[1][1] * c - tmp * s;
				mat[0][2] = (tmp = mat[0][2]) * c + mat[1][2] * s;
				mat[1][2] = mat[1][2] * c - tmp * s;
				mat[0][3] = (tmp = mat[0][3]) * c + mat[1][3] * s;
				mat[1][3] = mat[1][3] * c - tmp * s;
				flag |= ROTATION2D;
				return;
			}
		}
		else if (z == 0) {
			if (y < 0)
				s = -s;
			double tmp;
			mat[2][0] = (tmp = mat[2][0]) * c + mat[0][0] * s;
			mat[0][0] = mat[0][0] * c - tmp * s;
			mat[2][1] = (tmp = mat[2][1]) * c + mat[0][1] * s;
			mat[0][1] = mat[0][1] * c - tmp * s;
			mat[2][2] = (tmp = mat[2][2]) * c + mat[0][2] * s;
			mat[0][2] = mat[0][2] * c - tmp * s;
			mat[2][3] = (tmp = mat[2][3]) * c + mat[0][3] * s;
			mat[0][3] = mat[0][3] * c - tmp * s;
			flag |= ROTATION;
			return;
		}
	}
	else if (y == 0 && z == 0) {
		if (x < 0)
			s = -s;
		double tmp;
		mat[1][0] = (tmp = mat[1][0]) * c + mat[2][0] * s;
		mat[2][0] = mat[2][0] * c - tmp * s;
		mat[1][1] = (tmp = mat[1][1]) * c + mat[2][1] * s;
		mat[2][1] = mat[2][1] * c - tmp * s;
		mat[1][2] = (tmp = mat[1][2]) * c + mat[2][2] * s;
		mat[2][2] = mat[2][2] * c - tmp * s;
		mat[1][3] = (tmp = mat[1][3]) * c + mat[2][3] * s;
		mat[2][3] = mat[2][3] * c - tmp * s;
		flag |= ROTATION;
		return;
	}

	Vec3D v(x, y, z);
	v.Normalize();

	double ic = 1 - c;
	Mat4D rot;
	rot.mat[0][0] = v.x() * v.x() * ic + c;
	rot.mat[1][0] = v.x() * v.y() * ic - v.z() * s;
	rot.mat[2][0] = v.x() * v.z() * ic + v.y() * s;
	rot.mat[3][0] = 0.0f;
	rot.mat[0][1] = v.y() * v.x() * ic + v.z() * s;
	rot.mat[1][1] = v.y() * v.y() * ic + c;
	rot.mat[2][1] = v.y() * v.z() * ic - v.x() * s;
	rot.mat[3][1] = 0.0f;
	rot.mat[0][2] = v.x() * v.z() * ic - v.y() * s;
	rot.mat[1][2] = v.y() * v.z() * ic + v.x() * s;
	rot.mat[2][2] = v.z() * v.z() * ic + c;
	rot.mat[3][2] = 0.0f;
	rot.mat[0][3] = 0.0f;
	rot.mat[1][3] = 0.0f;
	rot.mat[2][3] = 0.0f;
	rot.mat[3][3] = 1.0f;
	flag |= ROTATION;
	(*this) *= rot;
}

void Mat4D::Translate(double x, double y, double z) {
	if (flag == IDENTITY) {
		mat[3][0] = x;
		mat[3][1] = y;
		mat[3][2] = z;
	}
	else if (flag == TRANSLATION) {
		mat[3][0] += x;
		mat[3][1] += y;
		mat[3][2] += z;
	}
	else if (flag == SCALE) {
		mat[3][0] = mat[0][0] * x;
		mat[3][1] = mat[1][1] * y;
		mat[3][2] = mat[2][2] * z;
	}
	else if (flag == (TRANSLATION | SCALE)) {
		mat[3][0] += mat[0][0] * x;
		mat[3][1] += mat[1][1] * y;
		mat[3][2] += mat[2][2] * z;
	}
	else if (flag < ROTATION) {
		mat[3][0] += mat[0][0] * x + mat[1][0] * y;
		mat[3][1] += mat[0][1] * x + mat[1][1] * y;
		mat[3][2] += mat[2][2] * z;
	}
	else {
		mat[3][0] += mat[0][0] * x + mat[1][0] * y + mat[2][0] * z;
		mat[3][1] += mat[0][1] * x + mat[1][1] * y + mat[2][1] * z;
		mat[3][2] += mat[0][2] * x + mat[1][2] * y + mat[2][2] * z;
		mat[3][3] += mat[0][3] * x + mat[1][3] * y + mat[2][3] * z;
	}
	flag |= TRANSLATION;
}