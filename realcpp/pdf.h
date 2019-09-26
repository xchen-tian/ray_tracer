#pragma once
#include "pch.h"
#include "common.h"
#include "ray.h"
#include "geometry.h"
#include "hitable.h"
#include "onb.h"
#include <iostream>


struct Pdf {

	virtual float probablity(const Ray & ray) const = 0;

	virtual Vec3 random(const HitRecord & record) const = 0;

};


struct RectLightPdf : public Pdf {

	float area;
	XZ_Rectangle * rect;
	RectLightPdf(XZ_Rectangle * rect): rect(rect) {
		area = rect->area();
	}

	virtual float probablity(const Ray & ray) const {
		// formula:  distant^2 / cos(alpha)
		HitRecord record;
		if (rect->hit_test(ray, record)) {
			auto cosine_alpha = record.cos_ray_normal;
			auto dist = ray.from - record.hit_point;
			return dist.squared_length() / (cosine_alpha * area);
		}
		else
			return 0.0f;
	}

	virtual Vec3 random(const HitRecord & record) const{
		//get a random point at surface
		Vec3 random_point = Vec3( rect->x0 + rand_next01()*(rect->x1 - rect->x0), 
			rect->y, 
			rect->z0 + rand_next01()*(rect->z1 - rect->z0));
		Vec3 dir = random_point - record.hit_point;
		return dir.to_unit_vector();
	}

};

struct LambertianPdf : public Pdf {

	Vec3 normal;
	Onb onb;

	LambertianPdf(const Vec3 & normal):normal(normal){
		onb.build_from_w(normal);
	}

	virtual float probablity(const Ray & ray) const {
		//cosine
		//return abs(normal.dot(ray.direction));
		//if (normal.dot(ray.direction) < 1e-8) //debug
		//	cout << "direction too small p:"<< normal.dot(ray.direction) << "\\\\\\" << normal << " " << ray.direction << endl;
		return abs(normal.dot(ray.direction));
	}

	virtual Vec3 random(const HitRecord & record) const {
		auto rand_direction = random_cosine_on_hemisphere();
		// project direction on surface
		Vec3 result = onb.local(rand_direction);


		//if (normal.dot(result) < 1e-8) //debug
		//	cout << "random angle too small = " << normal.dot(result) << "  \t\t|" << normal << " " << result << endl;

		return result;
	}

};

struct MixedPdf : public Pdf {

	Pdf * pdf1;
	Pdf * pdf2;

	MixedPdf(Pdf * pdf1, Pdf * pdf2) :pdf1(pdf1), pdf2(pdf2) {}

	virtual float probablity(const Ray & ray) const {
		//cosine
		float result = 0.5 * pdf1->probablity(ray) + 0.5 * pdf2->probablity(ray);
		return result;
	}

	virtual Vec3 random(const HitRecord & record) const {
		float r = rand_next01();
		if (r < 0.995f) // change back to 0.5
		{
			//cout << " ~~~~~~~~~~~~~> light" << endl;
			return pdf1->random(record); //light
		}
		else 
			return pdf2->random(record); //non-light
	}

};


