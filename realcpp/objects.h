#pragma once
#include "pch.h"
#include "common.h"
#include "material.h"
#include "ray.h"



struct Object {
	Sphere sphere;
	Material* material;
	Object(const Sphere& sphere, Material* material) : sphere(sphere), material(material) {}
};