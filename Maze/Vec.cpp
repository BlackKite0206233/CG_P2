#include "Vec.h"

Vec::Vec(int n): size(n) {
	vec = vector<double>(n);
}

Vec::Vec(vector<double> v) : vec(v) {
	size = v.size();
};

double Vec::Length() {
	double res = 0;
	for (auto& i : vec) {
		res += i * i;
	}
	return sqrt(res);
}

void Vec::Normalize() {
	(*this) /= Length();
}

Vec Vec::Normalized() {
	return (*this) / Length();
}

Vec operator+(const Vec& v1, const Vec& v2) {
	if (v1.size != v2.size) {
		throw "error";
	}
	Vec res(v1.size);
	for (int i = 0; i < v1.size; i++) {
		res.vec[i] = v1.vec[i] + v2.vec[i];
	}
	return res;
}

Vec operator-(const Vec& v1, const Vec& v2) {
	if (v1.size != v2.size) {
		throw "error";
	}
	Vec res(v1.size);
	for (int i = 0; i < v1.size; i++) {
		res.vec[i] = v1.vec[i] - v2.vec[i];
	}
	return res;
}

Vec operator*(const Vec& v, double n) {
	Vec res(v.size);
	for (int i = 0; i < v.size; i++) {
		res.vec[i] = v.vec[i] * n;
	}
	return res;
}

Vec operator*(double n, const Vec& v) {
	return v * n;
}

Vec operator/(const Vec& v, double n) {
	Vec res(v.size);
	for (int i = 0; i < v.size; i++) {
		res.vec[i] = v.vec[i] / n;
	}
	return res;
}

Vec operator/(double n, const Vec& v) {
	return v / n;
}

Vec Vec::operator+=(const Vec& v) {
	if (size != v.size) {
		throw "error";
	}
	for (int i = 0; i < size; i++) {
		vec[i] += v.vec[i];
	}
}

Vec Vec::operator-=(const Vec& v) {
	if (size != v.size) {
		throw "error";
	}
	for (int i = 0; i < size; i++) {
		vec[i] -= v.vec[i];
	}
}

Vec Vec::operator*=(double n) {
	for (int i = 0; i < size; i++) {
		vec[i] *= n;
	}
}

Vec Vec::operator/=(double n) {
	for (int i = 0; i < size; i++) {
		vec[i] *= n;
	}
}