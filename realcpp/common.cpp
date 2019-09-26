#include "pch.h"
#include "common.h"
#include <random>
using namespace std;

std::random_device dev;
std::mt19937 rng(dev());
uniform_real_distribution<float> rand_gen(-0.5f, 0.5f);

float rand_next() {
	return rand_gen(rng);
}

float rand_next01() {
	auto a = rand_gen(rng) + 0.5f;
	if (a < 0.0f) return 0.0f;
	if (a > 1.0f) return 1.0f;
	return a;
}