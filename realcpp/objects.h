#pragma once
#include "pch.h"
#include "common.h"
#include "hitable.h"
#include "material.h"
#include <iostream>

#define PI 3.14159265f


struct XY_Rectangle : public Hitable {
	float x0, y0;
	float x1, y1;
	float z;
	XY_Rectangle() {}
	XY_Rectangle(const float & x0, const float & y0,
		const float & x1, const float & y1, const float & z) :x0(x0),y0(y0),x1(x1),y1(y1),z(z) {}

	//TODO have not handle normal for rectangle

	inline void hit_u_v(HitRecord& rec) const {
		//TODO normal vector also matters?
		// hit_point's must be between x0,x1   y is the same
		rec.v = (rec.hit_point.x() - x0) / (x1 - x0);
		rec.u = (rec.hit_point.y() - x0) / (y1 - y0);
		if (rec.u < 0)  cout << " rectangle rec.u < 0 " << endl;//debug
		if (rec.v < 0)  cout << " rectangle rec.v < 0 " << endl;//debug
	}

	inline bool hit_test(const Ray& ray, HitRecord& rec) const {
		// 2D surface equation in 3D space
		// Ax + By + Cz + D = 0
		// in axis aligned ( to surface z = ??) case
		// ray function:
		//  x = x0 + at
		//  y = y0 + bt
		//  z = z0 + ct
		//  since z is fixed, then we can get t, then we get x and y, judge the range of x, y with hit point
		//  
		const float T_MIN = 1e-4f;
		const float T_MAX = 1e20f;
		if (abs(ray.direction.z()) > 1e-8) {
			float t = ( this->z - ray.from.z()) / ray.direction.z();
			auto p = ray.point_at(t);
			if (this->x0 <= p.x() && p.x() <= this->x1  &&
				this->y0 <= p.y() && p.y() <= this->y1) {
				rec.hit_point = p;
				rec.t = t;
				float z = ray.direction.z() > 0 ? -1.0f : 1.0f;
				rec.normal = Vec3(0, 0, z);
				rec.hitted = (void *)this;
				hit_u_v(rec);
				return true;
			} 
		}
		else { // perpendicular to z axis
			return false;
		}
		return false;
	}
	static const int thickness = 1e-8f;
	virtual AABB bounding_box() const {
		AABB aabb = AABB({x0,y0,z - thickness}, { x1,y1,z + thickness });
		return aabb;
	}

};

struct Sphere : public Hitable {
	Vec3 center;
	float radius;
	Sphere() {}
	Sphere(const Vec3&center, const float & radius) :center(center), radius(radius) {}

	inline void hit_u_v(HitRecord& rec) const {
		float phi = atan2(rec.normal.z(),rec.normal.x());  // - PI  ...  PI
		float theta = asin(rec.normal.y());
		phi -= PI;

		if (phi >= PI)
			phi -= 2 * PI;
		if (phi <= - PI)
			phi += 2 * PI;

		rec.v = 1.0f - (phi + PI) / (2 * PI);
		rec.u = (theta + PI/2)/PI;
	}

	inline bool hit_test(const Ray& ray, HitRecord& rec) const {
		const float T_MIN = 1e-4f;
		const float T_MAX = 1e20f;
		Vec3 oc = ray.from - this->center;
		float a = ray.direction.squared_length();
		float b = oc.dot(ray.direction);
		float c = oc.squared_length() - radius * radius;
		float discriminant = b * b - a * c;
		if (discriminant > 0) {
			float temp = (-b - sqrt(discriminant)) / a;
			if (temp < T_MAX && temp > T_MIN) {
				rec.t = temp;
				rec.hit_point = ray.point_at(rec.t);
				rec.normal = (rec.hit_point - center) / radius;
				rec.hitted = (void *)this;
				hit_u_v(rec);
				return true;
			}
			temp = (-b + sqrt(discriminant)) / a;
			if (temp < T_MAX && temp > T_MIN) {
				rec.t = temp;
				rec.hit_point = ray.point_at(rec.t);
				rec.normal = (rec.hit_point - center) / radius;
				rec.hitted = (void *)this;
				hit_u_v(rec);
				return true;
			}
		}
		return false;
	}

	virtual AABB bounding_box() const {
		AABB aabb = AABB(
			center - radius, center + radius);
		return aabb;
	}

};

struct Object {
	Hitable *hitable;
	Material* material;
	Object(Hitable* hitable, Material* material) : hitable(hitable), material(material) {}
};