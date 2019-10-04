#pragma once
#include <vector>
#include "Vec.h"

using namespace std;

class Mat4D;
class Mat {
public:
	friend class Mat4D;
	Mat() {};
	Mat(int);
	double Det();
	Mat Inverted();
	Mat CofatorMatrix(int, int);
	void SetToIdentity();


	friend Mat operator+(const Mat&, const Mat&);
	friend Mat operator-(const Mat&, const Mat&);
	friend Mat operator*(const Mat&, const Mat&);
	friend Vec operator*(const Mat&, const Vec&);
	friend Vec operator*(const Vec&, const Mat&);
	friend Mat operator*(const Mat&, double);
	friend Mat operator/(const Mat&, double);

	Mat& operator+=(const Mat&);
	Mat& operator-=(const Mat&);
	Mat& operator*=(const Mat&);
	Mat& operator*=(double);
	Mat& operator/=(double);

	int size = 0;
protected:
	vector<vector<double>> mat;
	enum {
		IDENTITY    = 0x0000,
		TRANSLATION = 0x0001,
		SCALE       = 0x0002,
		ROTATION2D  = 0x0004,
		ROTATION    = 0x0008,
		PERSPECTIVE = 0x0010,
		GENERAL     = 0x001f
	};
	int flag = GENERAL;
};

