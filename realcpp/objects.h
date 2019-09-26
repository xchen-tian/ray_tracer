#pragma once
#include "pch.h"
#include "geometry.h"
#include "material.h"
#include <iostream>


struct Object {
	Hitable *hitable;
	Material* material;
	Object(Hitable* hitable, Material* material) : hitable(hitable), material(material) {}
};

