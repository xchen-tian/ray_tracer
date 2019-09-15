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
		const float & x1, const float & y1, const float & z) :
		x0(x0),y0(y0),x1(x1),y1(y1),z(z) {}

	//TODO have not handle normal for rectangle

	inline void hit_u_v(HitRecord& rec) const {
		//TODO normal vector also matters?
		// hit_point's must be between x0,x1   y is the same
		rec.v = (rec.hit_point.x() - x0) / (x1 - x0);
		rec.u = (rec.hit_point.y() - y0) / (y1 - y0);
		if (rec.v < 0) {
			cout << rec.v << endl;
			cout << rec.hit_point.x() << "  " << x0 << " | " << x1 << endl;
			cout << " rectangle rec.v < 0 " << endl;//debug
		}
		if (rec.u < 0) {
			cout << rec.u << endl;
			cout << rec.hit_point.y() <<"  " << y0<<" | "<<y1 << endl;
			cout << " rectangle rec.v < 0 " << endl;//debug
		}
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
			if (t < T_MIN || t > T_MAX) return false;
			//cout << "t: " << t << endl;
			auto p = ray.point_at(t);
			if (this->x0 <= p.x() && p.x() <= this->x1  &&
				this->y0 <= p.y() && p.y() <= this->y1) {
				rec.hit_point = p;
				rec.t = t;
				float z = ray.direction.z() > 0 ? -1.0f : 1.0f;
				rec.normal = Vec3(0, 0, z);
				//cout << this->z << " ray from z " << ray.from.z() << " "<<ray.direction.z()<<" " << t << endl;
				//cout << "rectangle hit " << rec.hit_point <<"  " << ray.direction << endl;

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

	// Inherited via Hitable
	inline Vec3 center_gravity() const {
		return center - radius;
	}

};

struct RotateObj : public Hitable {
	Hitable* hitable;
	Matrix3  matrix; 
	Matrix3  inverse_m;
	RotateObj( const Matrix3 & m3 , 
		const Matrix3& i_m3, 
		Hitable * hitable) :hitable(hitable), matrix(m3), inverse_m(i_m3){}

	inline void hit_u_v(HitRecord& rec) const {
		hitable->hit_u_v(rec);
	}

	inline bool hit_test(const Ray& ray, HitRecord& rec) const {

		Ray ray2 = Ray(inverse_m * ray.from , inverse_m * ray.direction);

		//cout << ray.from << " - " << c << " = " << ray.from - c << endl;
		//cout << " rotated " << inverse_m * (ray.from - c) << "  shifted " << ray2.from << endl;
		
		if (hitable->hit_test(ray2, rec)) {
			//hit_u_v(rec);
			rec.hitted = (void *)this;
			//cout << " hit place " << rec.hit_point << endl;//debug
			rec.normal = matrix * rec.normal;
			rec.hit_point = matrix * rec.hit_point;
			return true;
		}
		return false;
	}

	virtual AABB bounding_box() const {
		auto aabb = hitable->bounding_box();
		auto aabb2 = AABB(matrix * aabb.start, matrix * aabb.end);
		Vec3 minp( aabb.start), maxp(aabb.end);
		
		auto v = aabb2.all_points();
		for (int i = 0; i < 3; i++) {
			for (const auto& j : v) {
				minp[i] = min(minp[i], j[i]);
				maxp[i] = max(maxp[i], j[i]);
			}
		}
		cout << "new aabb " << minp << "  ---  " << maxp << endl; // debug
		return AABB(minp, maxp);
	}

};



struct TranslateObj : public Hitable {
	Hitable* hitable;
	Vec3  transition;
	TranslateObj(const Vec3& vec,
		Hitable* hitable) :hitable(hitable), transition(vec) {}

	inline void hit_u_v(HitRecord& rec) const {
		hitable->hit_u_v(rec);
	}

	inline bool hit_test(const Ray& ray, HitRecord& rec) const {
		//cout << " hit test here" << endl;
		Ray ray2 = Ray(ray.from - this->transition, ray.direction);
		if (hitable->hit_test(ray2, rec)) {
			//hit_u_v(rec);
			rec.hitted = (void*)this;
			rec.hit_point += this->transition;
			return true;
		}
		return false;
	}

	virtual AABB bounding_box() const {
		auto aabb = hitable->bounding_box();
		return AABB(aabb.start + transition, aabb.end + transition);
	}

};

struct BoxObj : public Hitable {
	const float T_MIN = 1e-4f;
	const float T_MAX = 1e20f;
	Vec3 size;
	Hitable * rectangles[6];
	BoxObj(Vec3 size) :
		size(size) {
		auto* face_tiny = new XY_Rectangle(0, 0, 0.01, 0.01, 0);
		for (int i = 0; i < 6; i++)
			rectangles[i] = face_tiny;

		auto * face_xy = new XY_Rectangle(0, 0, size[0], size[1], 0);
		rectangles[0] = face_xy;
		rectangles[1] = new TranslateObj({ 0,0,size[2] },
			face_xy);


		auto face_xz = new XY_Rectangle(0, 0, size[0], size[2] , 0);
		rectangles[2] = new RotateObj(
			Matrix3::rotation_matrix_x(-PI / 2),
			Matrix3::rotation_matrix_x(PI / 2),
			face_xz);
		rectangles[3] = new TranslateObj({ 0,size[1],0 }, rectangles[2]);
		auto face_yz = new XY_Rectangle(0, 0, size[2], size[1], 0);
		rectangles[4] = new RotateObj(
			Matrix3::rotation_matrix_y(PI / 2),
			Matrix3::rotation_matrix_y(-PI / 2),
			face_yz);
		rectangles[5] = new TranslateObj({ size[0], 0, 0 }, rectangles[4]);
		//rectangles[5] = rectangles[4];
		//rectangles[5] = face_yz;
	}
	
	inline void hit_u_v(HitRecord& rec) const {
	}

	inline void hit_u_v(Hitable * p,HitRecord& rec) const {
		p->hit_u_v(rec);
	}

	inline bool hit_test(const Ray& ray, HitRecord& rec) const {
		//cout << " hit test here" << endl;
		float t = T_MAX;
		HitRecord rec2;
		for (const auto& i : rectangles) {
			if (i->hit_test(ray, rec)) {
				if (rec.t < t) {
					t = rec.t;
					rec2 = rec;
				}
			}
		}
		if (T_MIN < t && t < T_MAX) {
			rec = rec2;
			rec.hitted = (void*)this; // for material
			return true;
		}
		return false;
	}

	virtual AABB bounding_box() const {
		auto ab0 = rectangles[0]->bounding_box();
		Vec3 minp(ab0.start), maxp(ab0.end);

		for (const auto& j : rectangles) {
			auto ab = j->bounding_box();
			auto start = ab.start;
			auto end = ab.end;
			for (int i = 0; i < 3; i++) {
				minp[i] = min(minp[i], start[i]  );
				maxp[i] = max(maxp[i],   end[i]);
			}
		}
		return AABB(minp, maxp);
	}


};

struct Object {
	Hitable *hitable;
	Material* material;
	Object(Hitable* hitable, Material* material) : hitable(hitable), material(material) {}
};

