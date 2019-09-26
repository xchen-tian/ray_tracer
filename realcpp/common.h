#pragma once

#include "pch.h"

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <algorithm>

#define PI 3.14159265358979f

using namespace std;

class Vec3 {

public:
	Vec3() {}
	Vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	inline const Vec3& operator+() const { return *this; }
	inline Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
	inline float operator[](int i) const { return e[i]; }
	inline float& operator[](int i) { return e[i]; }

	inline Vec3& operator+=(const Vec3 &v2);
	inline Vec3& operator-=(const Vec3 &v2);
	inline Vec3& operator*=(const Vec3 &v2);
	inline Vec3& operator/=(const Vec3 &v2);
	inline Vec3& operator*=(const float t);
	inline Vec3& operator/=(const float t);

	inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	inline void make_unit_vector();
	inline Vec3 to_unit_vector() const;

	inline float dot(const Vec3 & vec) const { 
		float s = this->e[0] * vec.e[0] +
			this->e[1] * vec.e[1] +
			this->e[2] * vec.e[2];
		return s;
	}

	//vec has to be unit vector
	inline Vec3 project_on_unit(Vec3 vec) const;

	inline Vec3 minvec(const Vec3 & vec) const {
		return { min(e[0],vec.e[0]), min(e[1],vec.e[1]), min(e[2],vec.e[2]) };
	}

	inline Vec3 maxvec(const Vec3 & vec) const {
		return { max(e[0],vec.e[0]), max(e[1],vec.e[1]), max(e[2],vec.e[2]) };
	}

	inline bool anyNan() const {
		return isnan(e[0]) || isnan(e[1]) || isnan(e[2]);
	}

	float e[3];
};



inline std::istream& operator>>(std::istream &is, Vec3 &t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator<<(std::ostream &os, const Vec3 &t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline void Vec3::make_unit_vector() {
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k; e[1] *= k; e[2] *= k;
}

inline Vec3 Vec3::to_unit_vector() const {
	Vec3 copy(*this);
	copy.make_unit_vector();
	return copy;
}

inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
	return Vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline Vec3 operator+(const Vec3 &v1, const float & v) {
	return Vec3(v1.e[0] + v, v1.e[1] + v , v1.e[2] + v);
}

inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
	return Vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline Vec3 operator-(const Vec3 &v1, const float & v) {
	return Vec3(v1.e[0] - v, v1.e[1] - v, v1.e[2] - v);
}

inline Vec3 operator-(const float & v, const Vec3 &v1) {
	return -(v1 - v);
}

inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2) {
	return Vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
	return Vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline Vec3 operator*(float t, const Vec3 &v) {
	return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline Vec3 operator/(Vec3 v, float t) {
	return Vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline Vec3 operator*(const Vec3 &v, float t) {
	return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline float dot(const Vec3 &v1, const Vec3 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline Vec3 cross(const Vec3 &v1, const Vec3 &v2) {
	return Vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

inline Vec3& Vec3::operator+=(const Vec3 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

inline Vec3& Vec3::operator*=(const Vec3 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}

inline Vec3& Vec3::operator/=(const Vec3 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}

inline Vec3& Vec3::operator*=(const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline Vec3& Vec3::operator/=(const float t) {
	float k = 1.0 / t;

	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	return *this;
}

inline Vec3 unit_vector(Vec3 v) {
	return v / v.length();
}

inline Vec3 Vec3::project_on_unit(Vec3 vec) const {
	float scalar = this->dot(vec);
	Vec3 x = vec * scalar;
	return x;
}



struct Matrix3 {
	Vec3 rows[3];

	Matrix3(Vec3 v0, Vec3 v1, Vec3 v2) : rows{v0,v1,v2} {
	}

	inline Vec3 operator*(const Vec3& v2) const {
		Vec3 result;
		int i = 0;
		for (const auto& r : rows) {
			result[i] = r.dot(v2);
			i++;
		}
		return result;
	}

	static Matrix3 rotation_matrix_x(float theta) {
		return Matrix3(
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, cos(theta),  sin(theta) },
			{ 0.0f, -sin(theta), cos(theta) }
		);
	}

	static Matrix3 rotation_matrix_y(float theta) {
		auto a = Matrix3(
			{ cos(theta), 0.0f, -sin(theta) },
			{ 0.0f, 1.0f, 0.0f },
			{ sin(theta), 0.0f, cos(theta) }
		);
		//cout << a << endl; //debug
		return a;
	}
	
	static Matrix3 rotation_matrix_z(float theta) {
		return Matrix3(
			{  cos(theta), sin(theta),  0.0f },
			{ -sin(theta), cos(theta),  0.0f },
			{ 0.0f, 0.0f, 1.0f }
		);
	}

};

inline std::ostream& operator<<(std::ostream& os, const Matrix3 & m) {
	os << m.rows[0] << endl;
	os << m.rows[1] << endl;
	os << m.rows[2] << endl;
	return os;
}


float rand_next();
float rand_next01();

// integral cos(phi) * sin(phi) , dphi = [0, pi/2], dtheta = [0, 2pi]
// CDF = pi/2( 1 - cos(2*phi) ) / pi = sin^2(phi)
// inverse CDF:   sin(phi) = sqrt(cdf)
// integral = pi
// x = sin(phi) cos(theta)
// y = sin(phi) sin(theta)
// z = cos(phi) 
inline Vec3 random_cosine_on_hemisphere() {
	float r1 = rand_next01(); // about theta
	float r2 = rand_next01(); // cdf about  phi
	float z = sqrt(1 - r2);
	float theta = 2 * PI * r1;
	float x = cos(theta)*sqrt(r2);
	float y = sin(theta)*sqrt(r2);
	return Vec3(x, y, z);
}
