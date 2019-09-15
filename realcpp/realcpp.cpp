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

#define STB_IMAGE_IMPLEMENTATION    

#include "stb_image.h"
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
	// ----- sky background

	//const Vec3 top(0.5, 0.6, 0.8);
	//const Vec3 bottom(0.9, 0.9, 0.8);
	//y = 0.5*(y + 1.0);
	//return top *y + (1.0f - y)*bottom;

	// ----- dark background
	return { 0.0f,0.0f, 0.0f };

}

Vec3 normal_color(const Vec3 & v) {
	return 0.5 * (v.to_unit_vector() + 1.0f);
}

Hitable * root;

void build_bvh() {
	vector<Hitable *> h_list;
	for (auto & i : objs) {
		h_list.push_back(i.hitable);
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


	HitRecord record;
	hit = root->hit_test(ray, record);
	//HitRecord record(record2);

	if (hit) {
		//cout << " hit !!! "<< record.hitted << " at "<< record.hit_point << endl;//debug
		Ray ray_out;
		Vec3 decay;
		//Hitable * hitable = (Hitable *)record.hitted;
		Hitable * hitable = (Hitable *)record.hitted;
		auto * material = material_map[hitable];
		//cout <<  *(p->material) << " "<<record.hit_point<<" "<< record.t  << endl; // debug
		material->scatter(ray, record, decay, ray_out);
		if (material->self_luminous)
			return decay;
		return decay * render(ray_out, depth + 1);
	}
	//render background
	return background(ray.direction.y());
}

inline int colorint(float f) {
	auto a = int(255.99 * f);
	a = min(255, a);
	return a;
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

Texture* greywhite_tex = new ColorTexture({ 0.8, 0.8, 0.8 });

Texture *purple_tex = new ColorTexture({ 0.8, 0.5, 0.8 });
Texture *orange_tex = new ColorTexture({ 0.9, 0.5, 0.1 });
Texture *cyan_tex = new ColorTexture({ 0.1, 0.8, 0.8 });
Texture *white_tex = new ColorTexture({ 0.9, 0.9, 0.9 });

Texture *grid_tex = new GridTexture(white_tex,green_tex);

Material* mred = new Lambertian(red_tex); // red
Material* mgreen = new Lambertian(green_tex); // green
Material* mgrey = new Lambertian(grey_tex); // grey
Material* mgreywhite = new Lambertian(greywhite_tex); // grey

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

Texture*  earth_tex;
Material* material_earth;
Material* material_light = new Light(white_tex);

unsigned char * load_image(const char * path, int &col, int &row) {
	int channels;
	unsigned char * data; 
	data = stbi_load(path, &col, &row, &channels,0);
	cout << "loaded image " << path << "   row x col x channel: " << row << " x " << col << " x " << channels << endl;
	return data;
}

void init_img_texture() {
	int row, col;
	auto * data = load_image("earth.jpg",col,row);
	earth_tex = new ImageTexture(data,row,col);
	material_earth = new Lambertian(earth_tex);
}

void create_scene2() {

	Sphere *s1 = new Sphere({ 0, 0, 5 }, 2);
	Sphere *s2 = new Sphere({ 5, 0, 5 }, 2);
	Sphere *s3 = new Sphere({ -4, 0, 5 }, 2);
	Sphere *ground = new Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,iron),
		Object(s3,material_marble),
		Object(ground,material_marble)
	};
	camera2 = Camera({ -4,2,-3 },
		{ 13,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);
}

void create_scene_earth() {

	Sphere * s1 = new Sphere({ 0, 3, 5 }, 5);
	Sphere * ground = new Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,material_earth),
		Object(ground,mgrey)
	};
	camera2 = Camera({ -4,2,-3 },
		{ 13,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);
}


void create_scene_glass_ball() {

	Sphere * s1 = new Sphere({ 0, -0.2, 4 }, 1.5);
	Sphere * s2 = new Sphere({ 2.5, 0, 8 }, 2);
	Sphere * s3 = new Sphere({ -2.5, 0, 6 }, 2);
	Sphere * ground = new Sphere({ 0,-1400.05 - 2,5 }, 1400);

	objs = {
		Object(s1,glass2),
		Object(s2,mred),
		Object(s3,gold),
		Object(ground,mgrey)
	};
}

void cornel_box() {
	// create a -10 ~ 10 box
	XY_Rectangle* rect_wall = new XY_Rectangle(0, 0, 10, 10, 0);
	XY_Rectangle* rect_left = new XY_Rectangle(*rect_wall);
	XY_Rectangle* rect_right = new XY_Rectangle(*rect_wall);
	XY_Rectangle* rect_floor = new XY_Rectangle(0, 0, 20, 10, 0);

	rect_wall->z = 10;
	rect_wall->x0 = -10;
	auto r_left = Matrix3::rotation_matrix_y(90.0 / 180 * PI);
	auto i_r_left = Matrix3::rotation_matrix_y(-90.0 / 180 * PI);

	auto r_x= Matrix3::rotation_matrix_x(90.0 / 180 * PI);
	auto i_r_x= Matrix3::rotation_matrix_x(-90.0 / 180 * PI);
	
	XY_Rectangle* rect_light = new XY_Rectangle(-3, -3, 3, 3, 10);
	auto* left = new TranslateObj({ -10,0,0 }, 
		new RotateObj( r_left,i_r_left , rect_left));

	auto* right = new TranslateObj({ 10,0,10 },
		new RotateObj(i_r_left, r_left, rect_right));

	auto* l = new TranslateObj({ 0, -0.001,5 },
		new RotateObj(r_x, i_r_x, rect_light));

	auto* fl = new TranslateObj({ -10,0,10 },
		new RotateObj(r_x, i_r_x, rect_floor));

	auto* cei = new TranslateObj({ -10,10,10 },
		new RotateObj(r_x, i_r_x, rect_floor));

	auto* box_1= new TranslateObj({ -5.5,0,4 },
		new BoxObj({ 4,7,4} ));

	auto* box_2 = new TranslateObj({ 3,0,1 },
		new RotateObj( Matrix3::rotation_matrix_y(PI/6), Matrix3::rotation_matrix_y( -PI / 6),
		new BoxObj({ 2,3,2 })));

	auto* sphere = new Sphere({ 3,2,-1 }, 1.7);

	objs = {
		Object( left , mgreen),
		Object( rect_wall, mgreywhite),
		Object( right, mred),
		Object( fl, mgreywhite),
		Object( cei, mgreywhite),
		Object( box_1, mgrey),
		//Object( box_2, mgrey),
		Object(sphere, glass),
		Object( l, material_light)
	};

	cout << " fl  addr " << fl  << endl;
	cout << " cei addr " << cei << endl;
	cout << " l   addr " << l   << endl;

	cout << " right addr " << right << endl;
	cout << " left   addr " << left << endl;
	
	camera2 = Camera({ 0, 5, -5.5 }, { 0,5,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);

}

void create_box_scene() {
	BoxObj* box = new BoxObj({ 1,2,3 });
	auto* shift = new TranslateObj({ 3,0,0 }, box);
	objs = {
		//Object(box, mgreen)
		Object(shift, mgreen)
	};

	camera2 = Camera({ 0, 5, -5 }, { 0,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);
}

void create_scene_light_rect_and_ball() {

	Sphere * s1 = new Sphere({ 0, -0.2, 4 }, 1.5);
	Sphere * s2 = new Sphere({ -4, 0, 8 }, 2);
	XY_Rectangle * rect1 = new XY_Rectangle(0,0,10,10,0);
	auto rotate_y = Matrix3::rotation_matrix_y(-30.0 / 180.0 * PI);
	auto i_rotate_y = Matrix3::rotation_matrix_y(30.0 / 180.0 * PI);
	RotateObj* rot = new RotateObj( rotate_y, i_rotate_y, rect1 );
	TranslateObj* tans = new TranslateObj({0, 0 ,10}, rot);

	Sphere * ground = new Sphere({ 0,-1400.05 - 2,5 }, 1400);

	cout << " rot addr " << rot << endl;//debug

	objs = {
		Object(s1,glass),
		Object(s2,mred),
		//Object(rect1,material_light),
		//Object(rot,material_light),
		//Object(tans,material_light),
		Object(tans, mred),
		Object(ground,mgrey)
	};
	//camera2 = Camera({ -4,2,-3 },
	//	{ 13,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);

	camera2 = Camera({ 0, 2, 0 },{ 0,0,35 }, { 0,1.0f,0 }, 1.0f, 2.0f);
}


void create_scene_glass_random_balls() {

	Sphere * s1 = new Sphere({ 0,     10.2, 12 }, 15);
	Sphere * s2 = new Sphere({ 25,    5,   17 }, 10);
	Sphere * s3 = new Sphere({ -20.5, 5,   4 }, 5);
	Sphere * ground = new Sphere({ 0,-1404.05 - 2,5 }, 1400);

	objs = {
		Object(s1,glass),
		Object(s2,silver),
		Object(s3,gold),
		Object(ground,mground)
	};
	
	for (int i = 0; i < 20; i++) {
		Sphere * s;
		do {
			auto xpos = rand_next() * 2 * 20;
			auto zpos = rand_next() * 2 * 40 + 20;
			auto ypos = rand_next() * 4;
			auto radius = 1 + rand() * 2;
			s = new Sphere({ xpos,ypos,zpos },radius);
			bool flag = false;
			for (int j = 0; j < objs.size(); j++) {
				Sphere * s2 = dynamic_cast<Sphere *>(objs[j].hitable);
				float l = (s->center - s2->center).length();
				bool intersect = l < (s->radius + s2->radius);
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
				cout << " generated ball at " << s->center << " material " << m << endl;
				objs.push_back(temp);
				break;
			}
			else {
				delete s;
			}
		} while (true);
	}

}

void make_material_mapped() {
	for (auto & i : objs) {
		Hitable * addr = i.hitable;
		material_map[ addr] = i.material;
	}
}

void intersection_check() {
	int n = objs.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < i; j++) {
			Sphere * s1 = dynamic_cast<Sphere *>(objs[i].hitable);
			Sphere * s2 = dynamic_cast<Sphere *>(objs[j].hitable);
			float l = (s1->center - s2->center).length();
			bool intersect = l < (s1->radius + s2->radius);
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
	init_img_texture();
	//create_scene2();
	//create_scene_earth();
	//create_scene_light_rect_and_ball();
	cornel_box();
	//create_box_scene();
	//create_scene_glass_ball();
	//create_scene_glass_random_balls();
	build_bvh();
	cout << "-------------------- print BVH ----------------------" << endl;
	print_bvh(root,0);

	Vec3 screen_top   (0,  1, 0);
	Vec3 screen_bottom(0, -1, 0);
	Vec3 screen_left  (-2,  0, 0);
	Vec3 screen_right (2, 0, 0);
	Vec3 screen_z (0, 0, 1);
	int resolution_h = 300;
	int resolution_w = 600;

	//intersection_check();

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

			//if (w != 700 || h != 375) continue;
			
			Vec3 color{0,0,0};
			const int sample_num = 1500*5;
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
			color /= .3 * sample_num;

			//txtout << to.y() << "\t";
			fout << colorint(color.r()) << " " << colorint(color.g()) << " " << colorint(color.b()) << endl;
			//cout << colorint(color.r()) << " " << colorint(color.g()) << " " << colorint(color.b()) << endl; //debug
			counter += 1;
			if (counter % 500 == 0)
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
