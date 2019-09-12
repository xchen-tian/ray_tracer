#pragma once
#include "pch.h"
#include "common.h"
#include "ray.h"
#include "hitable.h"
#include "texture.h"
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
	bool self_luminous = false;


	inline Vec3 reflect(const Vec3& direction, const Vec3& normal) const {
		auto proj = direction.project_on_unit(normal);
		return direction - 2 * proj; // return value should be a unit vector 
	}
};

struct Lambertian :public Material {
	Texture *texture;
	Lambertian(Texture* texture) :texture(texture) {}

	virtual void scatter(const Ray& ray_in,
		const HitRecord& record,
		Vec3& decay,
		Ray& ray_out) const {
		decay = texture->value(record.u, record.v, Vec3(record.hit_point));
		ray_out.from = record.hit_point;
		ray_out.direction = this->random_ray_direction(record.normal);
	}

	virtual std::ostream& print(std::ostream& o ) const {
		o << "Lambertian: " << *texture;
		return o;
	}

};

struct Light:public Material {
	Texture *texture;
	Light(Texture* texture) :texture(texture) {
		self_luminous = true;
	}

	virtual void scatter(const Ray& ray_in,
		const HitRecord& record,
		Vec3& decay,
		Ray& ray_out) const {
		decay = texture->value(record.u, record.v, Vec3(record.hit_point));
		// in self_luminous material only decay is important
	}

	virtual std::ostream& print(std::ostream& o) const {
		o << "Lambertian: " << *texture;
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

struct Glass:public Material {
	Vec3 albedo;
	float fuzz;
	float ri; //refractive index
	Glass(const Vec3& albedo, 
		const float& fuzz,
		const float& refrative_index) :albedo(albedo), fuzz(fuzz), ri(refrative_index) {}

	// angle calculation
	// n1 * sin(t1) = n2 * sin(t2)   n1,n2 are refrative_index
	// n1 * ( (v - v.dot*(normal)) / |v|   ) = n2 * ( (v2 - v2.dot*(normal)) / |v2| )
	// but you don't know v2, you can only calculate sin(t2)
	// based on sin(t2), get unit vector t2

	inline bool refract(const Ray& ray_in, const Vec3 & normal, Vec3& out_direction) const{
		bool from_medium = ray_in.direction.dot(normal) > 0;
		float n1, n2;
		float sign = 1.0f; // control direction of normal 
		if (from_medium) {
			n1 = this->ri;
			n2 = 1.0f;
			sign = -1.0f;
			//cout << "from_medium" << endl;
		}
		else {
			n1 = 1.0f;
			n2 = this->ri;
		}
		auto neg_normal = - sign * normal;

		Vec3 vertical = ray_in.direction - ray_in.direction.project_on_unit(neg_normal);
		float sin_t1 = vertical.length() ; // Hypotenuse length = 1
		auto sin_t2 = n1 / n2 * sin_t1;
		if (sin_t2 > 1.0) { // total internal reflection
			return false;
		}
		// sin_t2 possibly > 1 TODO
		auto vertical2 = sin_t2;
		float cosine = sqrt(1 - vertical2 * vertical2);
		Vec3 dir_out = sin_t2 * vertical.to_unit_vector()
			+ cosine * neg_normal;
		out_direction = dir_out.to_unit_vector();
		return true;
	}

	virtual void scatter(const Ray& ray_in,
		const HitRecord& record,
		Vec3& decay,
		Ray& ray_out) const {
		decay = albedo;
		//cout << " metal reflect " << this->albedo << endl;

		ray_out.from = record.hit_point;
		bool refract = this->refract(ray_in, record.normal, ray_out.direction);
		if (!refract) {
			auto direction = this->reflect(ray_in.direction, record.normal);
			if (this->fuzz > 0) {
				direction = (1 - fuzz) * direction
					+ fuzz * this->random_ray_direction(record.normal);
			}
			ray_out.direction = direction;
		}
		else {
			//TODO add fuzz
			if (this->fuzz > 0) {
				auto& direction = ray_out.direction;
				direction = (1 - fuzz) * direction
					+ fuzz * this->random_ray_direction(record.normal);
			}
		}
	}

	virtual std::ostream& print(std::ostream& o) const {
		o << "Glass: " << albedo << " " << fuzz;
		return o;
	}

};


