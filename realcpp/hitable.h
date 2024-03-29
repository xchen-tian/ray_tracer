#pragma once

#include "pch.h"

#include "common.h"
#include "ray.h"
#include "aabb.h"



struct Hitable {
	virtual bool hit_test(const Ray& ray, HitRecord& rec) const = 0;
	virtual void hit_u_v(HitRecord& rec) const = 0;
	virtual AABB bounding_box() const = 0;
};

