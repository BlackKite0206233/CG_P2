#pragma once
#include <vector>
using namespace std;

class Vec3D;
class Vec {
public:
	friend class Vec3D;
	Vec() {};
	Vec(int n);
	Vec(vector<double> v);

	double Length();
	void Normalize();
	Vec Normalized();

	friend Vec operator+(const Vec&, const Vec&);
	friend Vec operator-(const Vec&, const Vec&);
	friend Vec operator*(const Vec&, double);
	friend Vec operator*(double, const Vec&);
	friend Vec operator/(const Vec&, double);
	friend Vec operator/(double, const Vec&);

	Vec operator+=(const Vec&);
	Vec operator-=(const Vec&);
	Vec operator*=(double);
	Vec operator/=(double);
protected:
	vector<double> vec;
	int size = 0;
};

