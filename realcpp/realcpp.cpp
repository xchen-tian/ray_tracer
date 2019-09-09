// realcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "common.h"
#include "material.h"
#include "ray.h"
#include "objects.h"
#include "camera.h"
#include "hitable.h"
#include "aabb.h"
#include "bvh_node.h"
#include "noise.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <map>
#include <omp.h>
#include <chrono>


using namespace std;

Vec3 origin(.0f, .0f, .0f);
Vec3 white(1.0f, 1.0f, 1.0f);
Vec3 red(.9f, .0f, .0f);

vector<Object> objs;
map< Hitable *,Material * > material_map;

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

Hitable * root;

void build_bvh() {
	vector<Hitable *> h_list;
	for (auto & i : objs) {
		h_list.push_back(&i.sphere);
	}
	root = BVHNode_build(h_list);
}


Vec3 render(Ray &ray,int depth) {
	// intersect with sphere closest
	if (depth > 50)
		return origin; // black

	//HitRecord record2;
	bool hit = false;
	Object* p = nullptr;
	HitRecord record2;

	// change this to bvh
	//for (auto & i : objs) { // has to be reference
	//	HitRecord record;
	//	if ( i.sphere.hit_test(ray,record) ) {
	//		if (!hit) {
	//			record2 = record;
	//			hit = true;
	//			record2.hitted = &i;
	//			p = &i;
	//		}
	//		else if (record.t < record2.t) {
	//			record2 = record;
	//			p = &i;
	//		}
	//	}
	//}

	HitRecord record;
	hit = root->hit_test(ray, record);
	//HitRecord record(record2);

	if (hit) {
		//cout << " hit !!! " << std::endl;
		Ray ray_out;
		Vec3 decay;
		//Hitable * hitable = (Hitable *)record.hitted;
		Hitable * hitable = (Hitable *)record.hitted;
		auto * material = material_map[hitable];
		//cout <<  *(p->material) << " "<<record.hit_point<<" "<< record.t  << endl; // debug
		material->scatter(ray, record, decay, ray_out);
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

PerlinNoise noise = PerlinNoise();
Texture *noise_tex = new NoiseTexture(&noise);
Texture *turb_tex = new TurbTexture(&noise);
Texture *marble_tex = new MarbleTexture(&noise);


Texture *red_tex = new ColorTexture({ 0.8, 0.3, 0.2 });
Texture *green_tex = new ColorTexture({ 0.3, 0.8, 0.2 });
Texture *grey_tex = new ColorTexture({ 0.5, 0.5, 0.5 });
Texture *purple_tex = new ColorTexture({ 0.8, 0.5, 0.8 });
Texture *orange_tex = new ColorTexture({ 0.9, 0.5, 0.1 });
Texture *cyan_tex = new ColorTexture({ 0.1, 0.8, 0.8 });
Texture *white_tex = new ColorTexture({ 0.9, 0.9, 0.9 });

Texture *grid_tex = new GridTexture(white_tex,green_tex);

Material* mred = new Lambertian(red_tex); // red
Material* mgreen = new Lambertian(red_tex); // green
Material* mgrey = new Lambertian(grey_tex); // grey
Material* mpurple = new Lambertian(purple_tex); // purple
Material* morange = new Lambertian(orange_tex); // orange
Material* mcyan = new Lambertian(cyan_tex); // cyan

Material* mground = new Lambertian(grid_tex); // grid color

Material* lambertians[] = { mred , mgreen , mgrey , mpurple ,morange ,mcyan };


Material* silver = new Metal({ 0.9, 0.9, 0.9 }, 0.0); // silver
Material* gold = new Metal({ 0.7, 0.6, 0.2 }, 0.0); // gold

Material* iron = new Metal({ 0.8, 0.8, 0.9 }, 0.2); // iron
Material* glass = new Glass({ 1.0, 1.0, 1.0 }, 0.0, 1.5); // iron
Material* glass2 = new Glass({ 1.0, 1.0, 1.0 }, 0.05, 1.3); // iron

Material* material_noise = new Lambertian(noise_tex); // iron
Material* material_turb = new Lambertian(turb_tex); // iron
Material* material_marble = new Lambertian(marble_tex); // iron



void create_scene2() {

	Sphere s1 = Sphere({ 0, 0, 5 }, 2);
	Sphere s2 = Sphere({ 5, 0, 5 }, 2);
	Sphere s3 = Sphere({ -4, 0, 5 }, 2);
	Sphere ground = Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,iron),
		Object(s3,material_marble),
		Object(ground,material_marble)
	};
	camera2 = Camera({ -4,2,-3 },
		{ 13,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);
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
		Object(ground,mground)
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

void make_material_mapped() {
	for (auto & i : objs) {
		Hitable * addr = &(i.sphere);
		material_map[ addr] = i.material;
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

void print_bvh(Hitable* node, int depth) {

	for (int i = 0; i < depth; i++)
		cout << "--";
	auto box = node->bounding_box();
	cout << box.start << " <-> " << box.end ;

	if (BVHNode* v = dynamic_cast<BVHNode*>(node)) {
		// old was safely casted to NewType
		cout << endl;
		print_bvh(v->left, depth + 1);
		print_bvh(v->right, depth + 1);
	}
	else {
		cout << " !"<<endl;
	}
}

void work() {
	create_scene2();
	//create_scene_glass_ball();
	//create_scene_glass_random_balls();
	build_bvh();
	print_bvh(root,0);

	Vec3 screen_top   (0,  1, 0);
	Vec3 screen_bottom(0, -1, 0);
	Vec3 screen_left  (-2,  0, 0);
	Vec3 screen_right (2, 0, 0);
	Vec3 screen_z (0, 0, 1);
	int resolution_h = 300;
	int resolution_w = 600;

	intersection_check();

	make_material_mapped();

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
	int counter = 0;
	for (int h = 0; h < resolution_h; h++) {
		for (int w = 0; w < resolution_w; w++) {
			
			Vec3 color{0,0,0};
			//const int sample_num = 150;
			const int sample_num = 20;
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
			counter += 1;
			if (counter % 300 == 0)
				cout <<"Progress : "<< 100.0 * counter / resolution_h / resolution_w << " %" << endl;
		}
		//txtout << endl;
	}
}

int main()
{
	auto start = chrono::system_clock::now();
	work();
	auto end = chrono::system_clock::now();
	chrono::duration<double> duration = end - start;
	cout << "program time cost: " << duration.count() << endl;
}
