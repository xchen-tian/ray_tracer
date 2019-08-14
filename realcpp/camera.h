#pragma once

#include "pch.h"

#include "common.h"
#include "ray.h"

struct Camera {

	Vec3 from;
	Vec3 lookat;
	Vec3 up;
	float vfov; // angle of top bottom ratio.
	float aspect; // angle of top bottom ratio.

	//create a surface, need 2 point
	//assume z is 1
	Vec3 rightdir;
	Vec3 zdir;

	Camera(Vec3 from, Vec3 lookat, Vec3 up, float vfov, float aspect):
		from(from), lookat(lookat), vfov(vfov), aspect(aspect){
		
		zdir = (lookat - from).to_unit_vector();
		auto up2 = up - up.project_on_unit(zdir); // make direction orthogonal
		this->up = up2;
		rightdir = cross(up2, zdir);
		up2.make_unit_vector();
		auto radian = vfov * 3.14159265 / 180.0;
		const float z_length = 1.0;
		float top = tanf(radian) * z_length;
		float left = top * aspect;
	}

	// change ray
	// assume ray's origin is (0,0,0),  to is from (-1,1)
	void transformRay(Ray & ray) {
		auto up_length = ray.direction.y();
		auto x_length  = ray.direction.x();
		Vec3 direction = up_length * this->up + x_length * this->rightdir + ray.direction.z() * zdir;
		ray.direction = direction.to_unit_vector();
		ray.from = this->from;
	}

};

