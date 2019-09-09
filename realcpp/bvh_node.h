#pragma once
#include "pch.h"
#include "common.h"
#include "hitable.h"
#include "aabb.h"
#include <iostream>

struct BVHNode : public Hitable {


	AABB box;
	Hitable * left;
	Hitable * right;
	int selected_axis;

	BVHNode(Hitable * left, Hitable * right) : left(left), right(right) {
		AABB a = left->bounding_box();
		AABB b = right->bounding_box();
		box = AABB(a.start.minvec(b.start), a.end.maxvec(b.end));
	}



	// Inherited via Hitable
	virtual AABB bounding_box() const {
		return this->box;
	}

	// Inherited via Hitable
	virtual bool hit_test(const Ray & ray, HitRecord & rec) const {
		if (!this->box.hit_test(ray)) return false;
		HitRecord hit_left;
		HitRecord hit_right;
		bool hit_l = left->hit_test(ray, hit_left);
		bool hit_r = right->hit_test(ray, hit_right);
		if (hit_l && hit_r) {
			if (hit_left.t <= hit_right.t)
				rec = hit_left;
			else
				rec = hit_right;
			return true;
		}
		else if (hit_l) {
			rec = hit_left;
			return true;
		}
		else if (hit_r) {
			rec = hit_right;
			return true;
		}
		else return false;
	}

};


template<int selected_axis>
inline 	bool compareHitable(const Hitable * a, const Hitable * b)
{
	return a->bounding_box().start.e[selected_axis] < b->bounding_box().start.e[selected_axis];
}


//build function

inline Hitable* BVHNode_build(vector<Hitable *> hits) {
	if (hits.size() == 0) { // should never reach
		return nullptr;
	}

	if (hits.size() == 1) {
		return hits[0];
	}

	if (hits.size() == 2) {
		BVHNode * x = new BVHNode(hits[0], hits[1]);
		return x;
	}

	// random an axis
	int axis = 0;
	float r = rand_next() + 0.5;
	if (r < 0.3333) {
		std::sort(hits.begin(), hits.end(), compareHitable<0>);
	}
	if (r < 0.3333 * 2) {
		std::sort(hits.begin(), hits.end(), compareHitable<1>);
	}
	else
		std::sort(hits.begin(), hits.end(), compareHitable<2>);

	vector<Hitable *> v_l(hits.begin(), hits.begin() + hits.size() / 2);
	vector<Hitable *> v_r(hits.begin() + hits.size() / 2, hits.end());

	Hitable * left = BVHNode_build(v_l);
	Hitable * right = BVHNode_build(v_r);
	BVHNode * x = new BVHNode(left, right);
	cout << " r " << r << " " << x->box.start <<" <->"<<x->box.end << endl;//debug
	return x;
}



