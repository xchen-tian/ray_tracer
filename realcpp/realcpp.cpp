// realcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "common.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>

using namespace std;

Vec3 origin(.0f, .0f, .0f);
Vec3 red(.9f, .0f, .0f);

vector<Sphere> spheres;

Vec3 background(float y) {
	const Vec3 top(0.5, 0.6, 0.8);
	const Vec3 bottom(0.9, 0.9, 0.8);
	y = 0.5*(y + 1.0);
	return top *y + (1.0f - y)*bottom;
}

Vec3 normal_color(const Vec3 & v) {
	return 0.5 * (v.to_unit_vector() + 1.0f);
}

struct HitRecord {
	float t; // the time of ray hits align the direction
	Vec3 hit_point;
	Vec3 normal;
};

struct Ray {
	Vec3 from;
	Vec3 direction;

	Ray(const Vec3 & from, const Vec3 & direction): from(from), direction(direction.to_unit_vector()){}
	Ray() {}

	Vec3 point_at(float t) {
		return t * direction + from;
	}

	bool hit(HitRecord & record, Sphere & sphere) {
		//1. project vector from ---> center to ray's direction
		//2. get perpendicular line length
		//3. compare it with radius
		//4. to get time t, need to get perpendicular point to hit point position (secant).
		//5. t = origin length - secant length (half)
		auto t1 = sphere.center - this->from;
		bool hit = false;

		auto proj = t1.project_on_unit(this->direction); // projection
		auto perpen = t1 - proj;
		hit = perpen.squared_length() < sphere.radius *sphere.radius;
		// positive angle < 90 degree
		if (t1.dot(direction) < 0.0f) {
			hit = false;
		}
		if (hit) {
			const auto r = sphere.radius;
			auto secant_length_half = sqrt(r*r - perpen.squared_length());
			float t = proj.length() - secant_length_half;
			record.hit_point = this->point_at(t);
			record.t = t;
			record.normal = - t1 + (record.hit_point - from);
			record.normal.make_unit_vector();
		}
		return hit;

	}

};

Vec3 render(Ray &ray) {
	// intersect with sphere

	for (auto i : spheres) {
		HitRecord record;
		if (ray.hit(record,i)) {
			//cout << ray.direction << endl;
			auto newray = Ray();
			//TODO -------------------------------- here

			//return normal_color(record.normal);
			//return red;
		}
	}

	//render background
	return background(ray.direction.y());
}

inline int colorint(float f) {
	return int(255.99 * f);
}

void create_scene1() {
	//Sphere s1 = Sphere({1,1,4},1.07);
	Sphere s1 = Sphere({ 0.5,0,6 }, 3);
	Sphere ground = Sphere({ 0,400.5,10 }, 400);
	spheres.push_back(s1);
	spheres.push_back(ground);
}

void work() {
	create_scene1();
	Vec3 screen_top   (0,  1, 0);
	Vec3 screen_bottom(0, -1, 0);
	Vec3 screen_left  (-2,  0, 0);
	Vec3 screen_right (2, 0, 0);
	Vec3 screen_z (0, 0, 1);
	//int resolution_h = 100;
	//int resolution_w = 200;

	int resolution_h = 300;
	int resolution_w = 600;

	std::random_device dev;
	std::mt19937 rng(dev());
	uniform_real_distribution<float> rand_gen(-0.5f, 0.5f);

	ofstream fout;
	ofstream txtout;
	fout.open("generated.ppm", std::ofstream::out);
	txtout.open("generated_txt.txt", std::ofstream::out);
	fout << "P3" << endl;
	fout << resolution_w <<" " << resolution_h << endl;
	fout << 255 << endl;
	for (int h = 0; h < resolution_h; h++) {
		for (int w = 0; w < resolution_w; w++) {
			
			Vec3 color{0,0,0};
			const int sample_num = 20;
			for (int sample = 0; sample < sample_num; sample ++) {
				float rh = (h + rand_gen(rng)) * 1.0 / resolution_h;
				float rw = (w + rand_gen(rng)) * 1.0 / resolution_w;
				Vec3 to = (1 - rw) * screen_left + rw * screen_right +
					(1 - rh) * screen_top + rh * screen_bottom + screen_z;

				Ray ray(origin, to);
				Vec3 colorx = render(ray);
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

