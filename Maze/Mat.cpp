#include "Mat.h"

#define EPS 0.00000001

Mat::Mat(int n): size(n) {
	mat = vector<vector<double>>(n, vector<double>(n, 0));
}

double Mat::Det() {
	if (size == 1) {
		return mat[0][0];
	}
	if (size == 2) {
		return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	}

	double det = 0;
	for (int i = 0; i < size; i++) {
		det += pow(-1, i) * mat[i][0] * CofatorMatrix(i, 0).Det();
	}

	return det;
}

Mat Mat::Inverted() {
	double det = Det();
	Mat m = Mat(size);

	if (abs(det) < EPS) {
		return Mat(size);
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			m.mat[i][j] = CofatorMatrix(i, j).Det();
		}
	}
	return m / det;
}

Mat Mat::CofatorMatrix(int r, int c) {
	Mat m(size - 1);
	int countR = 0;
	int countC = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (!(i == r || j == c)) {
				m.mat[countR][countC] = mat[i][j];
				countC++;
				if (countC == size - 1) {
					countC = 0;
					countR++;
				}
			}
		}
	}
	return m;
}

void Mat::SetToIdentity() {
	mat = vector<vector<double>>(4, vector<double>(4, 0));
	for (int i = 0; i < 4; i++) {
		mat[i][i] = 1;
	}
	flag = IDENTITY;
}

Mat operator+(const Mat& m1, const Mat& m2) {
	if (m1.size != m2.size) {
		throw "error";
	}
	Mat res(m1.size);
	for (int i = 0; i < m1.size; i++) {
		for (int j = 0; j < m1.size; j++) {
			res.mat[i][j] = m1.mat[i][j] + m2.mat[i][j];
		}
	}
	return res;
}

Mat operator-(const Mat& m1, const Mat& m2) {
	if (m1.size != m2.size) {
		throw "error";
	}
	Mat res(m1.size);
	for (int i = 0; i < m1.size; i++) {
		for (int j = 0; j < m1.size; j++) {
			res.mat[i][j] = m1.mat[i][j] - m2.mat[i][j];
		}
	}
	return res;
}

Mat operator*(const Mat& m1, const Mat& m2) {
	if (m1.size != m2.size) {
		throw "error";
	}
	Mat res(m1.size);
	for (int i = 0; i < m1.size; i++) {
		for (int j = 0; j < m1.size; j++) {
			for (int k = 0; k < m1.size; k++) {
				res.mat[i][j] += m1.mat[i][k] * m2.mat[k][j];
			}
		}
	}
	return res;
}

Vec operator*(const Mat& m, const Vec& v) {
	if (m.size != v.size) {
		throw "error";
	}
	Vec res(v.size);
	for (int i = 0; i < m.size; i++) {
		for (int j = 0; j < m.size; j++) {
			res[i] += v[j] * m.mat[j][i];
		}
	}
	return res;
}

Vec operator*(const Vec& v, const Mat& m) {
	return m * v;
}

Mat operator*(const Mat& m, double n) {
	Mat res(m.size);
	for (int i = 0; i < m.size; i++) {
		for (int j = 0; j < m.size; j++) {
			res.mat[i][j] = m.mat[i][j] * n;
		}
	}
	return res;
}


Mat operator/(const Mat& m, double n) {
	Mat res(m.size);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res.mat[i][j] = m.mat[i][j] / n;
		}
	}
	return res;
}

Mat& Mat::operator+=(const Mat& m) {
	if (m.size != size) {
		throw "error";
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i][j] += m.mat[i][j];
		}
	}
	return *this;
}

Mat& Mat::operator-=(const Mat& m) {
	if (m.size != size) {
		throw "error";
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i][j] -= m.mat[i][j];
		}
	}
	return *this;
}

Mat& Mat::operator*=(const Mat& m) {
	if (m.size != size) {
		throw "error";
	}
	Mat res(size);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				res.mat[i][j] += mat[i][k] * m.mat[k][j];
			}
		}
	}
	mat = res.mat;
	return *this;
}

Mat& Mat::operator*=(double n) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i][j] *= n;
		}
	}
	return *this;
}

Mat& Mat::operator/=(double n) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i][j] /= n;
		}
	}
	return *this;
}