#pragma once
#include "pch.h"
#include "common.h"
#include "hitable.h"
#include "material.h"
#include <iostream>


struct Sphere : public Hitable {
	Vec3 center;
	float radius;
	Sphere() {}
	Sphere(const Vec3&center, const float & radius) :center(center), radius(radius) {}


	inline bool hit_test(const Ray& ray, HitRecord& rec) const {
		const float T_MIN = 1e-4;
		const float T_MAX = 1e20;
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
				return true;
			}
			temp = (-b + sqrt(discriminant)) / a;
			if (temp < T_MAX && temp > T_MIN) {
				rec.t = temp;
				rec.hit_point = ray.point_at(rec.t);
				rec.normal = (rec.hit_point - center) / radius;
				rec.hitted = (void *)this;
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
	Sphere sphere;
	Material* material;
	Object(const Sphere& sphere, Material* material) : sphere(sphere), material(material) {}
};