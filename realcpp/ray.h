#pragma once

#include "pch.h"
#include "common.h"

struct HitRecord {
	float t; // the time of ray hits align the direction
	Vec3 hit_point;
	Vec3 normal;
	float u, v;
	void *hitted = nullptr;
};


struct Ray {
	Vec3 from;
	Vec3 direction;

	Ray(const Vec3& from, const Vec3& direction) : from(from), direction(direction.to_unit_vector()) {}
	Ray() {}

	Vec3 point_at(float t) const {
		return t * direction + from;
	}


};