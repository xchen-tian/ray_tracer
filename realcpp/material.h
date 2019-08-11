#pragma once
#include "pch.h"
#include "common.h"
#include "ray.h"
#include <iostream>
using namespace std;

struct Material {

	virtual std::ostream& print(std::ostream&) const = 0;

	friend std::ostream& operator << (std::ostream& os, const Material& b) {
		return b.print(os);
	}

	virtual void scatter(const Ray& ray_in, 
		const HitRecord& record, 
		Vec3& decay, 
		Ray& ray_out) const = 0;
	inline Vec3 random_ray_direction(const Vec3& normal) const {
		Vec3 v = { rand_next(), rand_next(), rand_next() };
		if (v.dot(normal) > 0) {
			return v.to_unit_vector();
		}
		else {
			return (-v).to_unit_vector();
		}
	}



	inline Vec3 reflect(const Vec3& direction, const Vec3& normal) const {
		auto proj = direction.project_on_unit(normal);
		return direction - 2 * proj; // return value should be a unit vector 
	}
};

struct Lambertian :public Material {
	Vec3 albedo;
	Lambertian(const Vec3& albedo) :albedo(albedo) {}

	virtual void scatter(const Ray& ray_in,
		const HitRecord& record,
		Vec3& decay,
		Ray& ray_out) const {
		decay = albedo;
		ray_out.from = record.hit_point;
		ray_out.direction = this->random_ray_direction(record.normal);
	}

	virtual std::ostream& print(std::ostream& o ) const {
		o << "Lambertian: " << albedo;
		return o;
	}

};

struct Metal :public Material {
	Vec3 albedo;
	float fuzz;
	Metal(const Vec3& albedo, const float& fuzz) :albedo(albedo), fuzz(fuzz) {}

	virtual void scatter(const Ray& ray_in,
		const HitRecord& record,
		Vec3& decay,
		Ray& ray_out) const {
		decay = albedo;
		ray_out.from = record.hit_point;

		//cout << " metal reflect " << this->albedo << endl;

		auto direction = this->reflect(ray_in.direction, record.normal);
		if (this->fuzz > 0) {
			direction = (1 - fuzz) * direction
				+ fuzz * this->random_ray_direction(record.normal);
		}
		ray_out.direction = direction;
	}

	virtual std::ostream& print(std::ostream& o) const {
		o << "Metal: " << albedo <<" "<<fuzz;
		return o;
	}

};

