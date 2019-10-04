#pragma once
#include <vector>
using namespace std;

class Vec3D;
class Vec4D;
class Vec {
public:
	friend class Vec3D;
	friend class Vec4D;
	Vec() {};
	Vec(int n);
	Vec(vector<double> v);

	double Length();
	void Normalize();
	Vec Normalized();

	friend Vec operator+(const Vec&, const Vec&);
	friend Vec operator-(const Vec&, const Vec&);
	friend Vec operator*(const Vec&, double);
	friend Vec operator/(const Vec&, double);

	double& operator[](int);
	double operator[](int) const;
	Vec& operator+=(const Vec&);
	Vec& operator-=(const Vec&);
	Vec& operator*=(double);
	Vec& operator/=(double);

	int size = 0;
protected:
	vector<double> vec;
};

