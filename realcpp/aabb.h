#pragma once

#include "pch.h"

#include "common.h"
#include "ray.h"
#include "objects.h"
#include <algorithm>



struct AABB {

	Vec3 start, end;
	AABB(Vec3 start, Vec3 end) : start(start), end(end) {}
	AABB() {}

	inline bool parallel(float dir) const {
		return abs(dir) < 1e-9;
	}

	inline void swapmin(float & a, float & b) const {
		if (a < b) return;
		auto t = a;
		a = b;
		b = t;
	}

	inline bool hit_test(const Ray& ray) const {
		// p(t) = origin + direction * t;
		// for each dimension, find time range, tmin, tmax
		// find whether all dimension overlaps
		float tmin(1e19), tmax(-1e19);
		for (int i = 0; i < 3; i++) {
			float dir = ray.direction.e[i];
			float b = ray.from.e[i];
			if (parallel(dir)) {
				if (!(start[i] <= b && b <= start[i]))
					return false;
				// all range statisfy, do not check tmin or tmax
			}
			else {
				// y = b + a*t;
				// (y-b)/a = t;
				float t0 = (start[i] - b) / dir;
				float t1 = (end[i] - b) / dir;
				swapmin(t0, t1);
				tmin = max(tmin, t0);
				tmax = min(tmax, t1);
			}
		}
		if (tmin > tmax) return false;

		return true;
	}

};
