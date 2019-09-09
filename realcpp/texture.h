#pragma once
#include "pch.h"
#include "common.h"
#include "noise.h"
#include <iostream>

struct Texture {
	virtual Vec3 value(float u, float v, const Vec3 & p) const = 0;

	friend std::ostream& operator << (std::ostream& os, const Texture& b) {
		return b.print(os);
	}

	virtual std::ostream& print(std::ostream&) const = 0;


};


struct ColorTexture :Texture {
	Vec3 color;
	ColorTexture(const Vec3 & color) : color(color) {}
	virtual Vec3 value(float u, float v, const Vec3 & p) const {
		return color;
	}
	virtual std::ostream& print(std::ostream& o) const {
		o << "ColorTexture" << this->color ;
		return o;
	}

};


struct GridTexture:Texture {
	Texture * odd, * even;
	GridTexture(Texture * odd, Texture * even) : odd(odd), even(even) {}
	virtual Vec3 value(float u, float v, const Vec3 & p) const {
		bool is_even = sin(0.2 * p.x()) * sin(0.2 * p.y()) * sin(0.2 * p.z()) < 0;
		if (is_even) 
			return odd->value(u, v, p);
		else 
			return even->value(u, v, p);
	}
	virtual std::ostream& print(std::ostream& o) const {
		o << "GridTexture (odd: "; odd->print(o) << ", even:"; even->print(o) << ")";
		return o;
	}
};


struct NoiseTexture :Texture {
	PerlinNoise *noise;
	NoiseTexture(PerlinNoise *n) : noise(n) {}
	virtual Vec3 value(float u, float v, const Vec3 & p) const {
		float t = noise->noise(p);
		return Vec3(t, t, t);
	}
	virtual std::ostream& print(std::ostream& o) const {
		o << "NoiseTexture()" ;
		return o;
	}

};

struct TurbTexture :Texture {
	PerlinNoise *noise;
	TurbTexture(PerlinNoise *n) : noise(n) {}
	virtual Vec3 value(float u, float v, const Vec3 & p) const {
		float t = noise->turb(p);
		return Vec3(t, t, t);
	}
	virtual std::ostream& print(std::ostream& o) const {
		o << "TurbTexture()";
		return o;
	}

};


struct MarbleTexture :Texture {
	PerlinNoise *noise;
	float scale = 1;
	MarbleTexture(PerlinNoise *n) : noise(n) {}
	virtual Vec3 value(float u, float v, const Vec3 & p) const {
		float n = scale * p.z() + 10 * noise->turb(p);
		return Vec3(1.f, 1.f, 1.f) * 0.5 *( 1 + sin(n)  );
	}
	virtual std::ostream& print(std::ostream& o) const {
		o << "MarbleTexture()";
		return o;
	}

};