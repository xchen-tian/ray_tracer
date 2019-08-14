// realcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "common.h"
#include "material.h"
#include "ray.h"
#include "objects.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <omp.h>


using namespace std;

Vec3 origin(.0f, .0f, .0f);
Vec3 white(1.0f, 1.0f, 1.0f);
Vec3 red(.9f, .0f, .0f);

vector<Object> objs;

Camera camera_origin = Camera(origin, {0,0,1.0f}, { 0,1.0f,0 }, 1.0f, 2.0f); 

Camera camera2 = Camera({-4,10,-20}, 
	{ 13,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);

Vec3 background(float y) {
	const Vec3 top(0.5, 0.6, 0.8);
	const Vec3 bottom(0.9, 0.9, 0.8);
	y = 0.5*(y + 1.0);
	return top *y + (1.0f - y)*bottom;
}

Vec3 normal_color(const Vec3 & v) {
	return 0.5 * (v.to_unit_vector() + 1.0f);
}



//bool hit_test(const Ray & ray, HitRecord& record, Object& objs) {
//	const float T_MIN = 1e-9;
//	const float T_MAX = 1e20;
//	//1. project vector from ---> center to ray's direction
//	//2. get perpendicular line length
//	//3. compare it with radius
//	//4. to get time t, need to get perpendicular point to hit point position (secant).
//	//5. t = origin length - secant length (half)
//	auto t1 = objs.sphere.center - ray.from;
//	bool hit = false;
//
//	auto proj = t1.project_on_unit(ray.direction); // projection
//	auto perpen = t1 - proj;
//	hit = perpen.squared_length() < objs.sphere.radius * objs.sphere.radius;
//	// positive angle < 90 degree
//	if (t1.dot(ray.direction) < 0.0f) {
//		hit = false;
//	}
//	if (hit) {
//		const auto r = objs.sphere.radius;
//		auto secant_length_half = sqrt(r * r - perpen.squared_length());
//		float t = proj.length() - secant_length_half;
//		if (!(T_MIN <= t)) {
//			return false;
//		}
//		record.hit_point = ray.point_at(t);
//		record.t = t;
//		record.normal = -t1 + (record.hit_point - ray.from);
//		record.normal.make_unit_vector();
//	}
//	return hit;
//
//}

bool hit_test(const Ray& ray, HitRecord& rec, Object& obj) {
	const float T_MIN = 1e-4;
	const float T_MAX = 1e20;
	Vec3 oc = ray.from - obj.sphere.center;
	float a = ray.direction.squared_length();
	float b = oc.dot(ray.direction);
	auto radius = obj.sphere.radius;
	float c = oc.squared_length() - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < T_MAX && temp > T_MIN) {
			rec.t = temp;
			rec.hit_point = ray.point_at(rec.t);
			rec.normal = (rec.hit_point - obj.sphere.center) / radius;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < T_MAX && temp > T_MIN) {
			rec.t = temp;
			rec.hit_point = ray.point_at(rec.t);
			rec.normal = (rec.hit_point - obj.sphere.center) / radius;
			return true;
		}
	}
	return false;
}

Vec3 render(Ray &ray,int depth) {
	// intersect with sphere closest
	if (depth > 50)
		return origin; // black

	HitRecord record2;
	bool hit = false;
	Object* p = nullptr;
	for (auto & i : objs) { // has to be reference
		HitRecord record;
		if (hit_test(ray,record,i)) {
			if (!hit) {
				record2 = record;
				hit = true;
				p = &i;
			}
			else if (record.t < record2.t) {
				record2 = record;
				p = &i;
			}
			//cout << ray.direction << endl;
			//return normal_color(record.normal);
			//return red;
		}
	}

	if (hit) {
		Ray ray_out;
		Vec3 decay;
		//cout <<  *(p->material) << " "<<record2.hit_point<<" "<< record2.t  << endl;
		p->material->scatter(ray, record2, decay, ray_out);
		return decay * render(ray_out, depth + 1);
	}

	//render background
	return background(ray.direction.y());

}

inline int colorint(float f) {
	return int(255.99 * f);
}

void create_scene1() {
	//Sphere s1 = Sphere({1,1,4},1.07);
	//Sphere s1 = Sphere({ 15.5, 0, 9 }, 3);
	Sphere s1 = Sphere({ 0, 0, 5 }, 2);
	Sphere ground = Sphere({ 0,-1400.05 - 2,5 }, 1400);
	//spheres.push_back(s1);
	//spheres.push_back(ground);
}

Material* mred = new Lambertian({ 0.8, 0.3, 0.2 }); // red
Material* mgreen = new Lambertian({ 0.3, 0.8, 0.2 }); // green
Material* mgrey = new Lambertian({ 0.5, 0.5, 0.5 }); // grey
Material* mpurple = new Lambertian({ 0.8, 0.5, 0.8 }); // purple
Material* morange = new Lambertian({ 0.9, 0.5, 0.1 }); // orange
Material* mcyan = new Lambertian({ 0.1, 0.8, 0.8 }); // cyan

Material* lambertians[] = { mred , mgreen , mgrey , mpurple ,morange ,mcyan };


Material* silver = new Metal({ 0.9, 0.9, 0.9 }, 0.0); // silver
Material* gold = new Metal({ 0.7, 0.6, 0.2 }, 0.0); // gold

Material* iron = new Metal({ 0.8, 0.8, 0.9 }, 0.2); // iron
Material* glass = new Glass({ 1.0, 1.0, 1.0 }, 0.0, 1.5); // iron
Material* glass2 = new Glass({ 1.0, 1.0, 1.0 }, 0.05, 1.3); // iron

void create_scene2() { 

	Sphere s1 = Sphere({ 0, 0, 5 }, 2);
	Sphere s2 = Sphere({ 5, 0, 5 }, 2);
	Sphere s3 = Sphere({ -4, 0, 5 }, 2);
	Sphere ground = Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,iron),
		Object(s2,silver),
		Object(s3,gold),
		Object(ground,mgrey)
	};
}

void create_scene_glass_ball() {

	Sphere s1 = Sphere({ 0, -0.2, 4 }, 1.5);
	Sphere s2 = Sphere({ 2.5, 0, 8 }, 2);
	Sphere s3 = Sphere({ -2.5, 0, 6 }, 2);
	Sphere ground = Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,glass2),
		Object(s2,mred),
		Object(s3,gold),
		Object(ground,mgrey)
	};
}

void create_scene_glass_random_balls() {

	Sphere s1 = Sphere({ 0,     10.2, 12 }, 15);
	Sphere s2 = Sphere({ 25,    5,   17 }, 10);
	Sphere s3 = Sphere({ -20.5, 5,   4 }, 5);
	Sphere ground = Sphere({ 0,-1404.05 - 2,5 }, 1400);

	objs = {
		Object(s1,glass),
		Object(s2,silver),
		Object(s3,gold),
		Object(ground,mgrey)
	};
	
	for (int i = 0; i < 20; i++) {
		Sphere s;
		do {
			auto xpos = rand_next() * 2 * 20;
			auto zpos = rand_next() * 2 * 40 + 20;
			auto ypos = rand_next() * 4;
			auto radius = 1 + rand() * 2;
			s = Sphere({ xpos,ypos,zpos },radius);
			bool flag = false;
			for (int j = 0; j < objs.size(); j++) {
				auto& s2 = objs[j].sphere;
				float l = (s.center - s2.center).length();
				bool intersect = l < (s.radius + s2.radius);
				if (intersect) {
					flag = true;
				}
			}
			if (!flag) {
				Material* m;
				auto r = rand_next();
				if (r < -0.4)
					m = gold;
				else if (r < -0.3)
					m = silver;
				else {
					r = r < 0 ? -r : r;
					auto r2 = (int(r * 12383721) + 779);
					m = lambertians[r2 % 6];
				}
				auto && temp = Object(s, m);
				cout << " generated ball at " << s.center << " material " << m << endl;
				objs.push_back(temp);
				break;
			}
		} while (true);
	}

}

void intersection_check() {
	int n = objs.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < i; j++) {
			auto& s1 = objs[i].sphere;
			auto& s2 = objs[j].sphere;
			float l = (s1.center - s2.center).length();
			bool intersect = l < (s1.radius + s2.radius);
			if (intersect) {
				cout << i << " intersect with " << j << endl;
			}
		}
	}
}

void work() {
	//create_scene2();
	//create_scene_glass_ball();
	create_scene_glass_random_balls();
	Vec3 screen_top   (0,  1, 0);
	Vec3 screen_bottom(0, -1, 0);
	Vec3 screen_left  (-2,  0, 0);
	Vec3 screen_right (2, 0, 0);
	Vec3 screen_z (0, 0, 1);
	int resolution_h = 300;
	int resolution_w = 600;

	intersection_check();

	resolution_h = 300 * 2;
	resolution_w = 600 * 2;

	ofstream fout;
	ofstream txtout;
	fout.open("generated.ppm", std::ofstream::out);
	txtout.open("generated_txt.txt", std::ofstream::out);
	fout << "P3" << endl;
	fout << resolution_w <<" " << resolution_h << endl;
	fout << 255 << endl;

	//#pragma omp parallel for
	for (int h = 0; h < resolution_h; h++) {
		for (int w = 0; w < resolution_w; w++) {
			
			Vec3 color{0,0,0};
			const int sample_num = 150;
			//const int sample_num = 20;
			for (int sample = 0; sample < sample_num; sample ++) {
				float rh = (h + rand_next()) * 1.0 / resolution_h;
				float rw = (w + rand_next()) * 1.0 / resolution_w;
				Vec3 to = (1 - rw) * screen_left + rw * screen_right +
					(1 - rh)*screen_top + rh* screen_bottom + screen_z;

				Ray ray(origin, to);
				//camera_origin.transformRay(ray);
				camera2.transformRay(ray);
				Vec3 colorx = render(ray,0);
				color += colorx;
			}
			color /= 1.0 * sample_num;

			//txtout << to.y() << "\t";
			fout << colorint(color.r()) << " " << colorint(color.g()) << " " << colorint(color.b()) << endl;
		}
		//txtout << endl;
	}
}

int main()
{
	work();
}
