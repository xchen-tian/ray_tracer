#pragma once
#include "common.h"
#include <iostream>




struct PerlinNoise {

	unsigned char perm[256] = { 151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
  129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
  49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

	unsigned char perm2[256];
	unsigned char perm3[256];

	void gen_perm(unsigned char *p, int scale, int offset) {
		for (int i = 0; i < 256; i++) {
			int idx2 = scale * (i+17) + offset;
			idx2 = idx2 % 256;
			p[idx2] = i;
		}
	}

	PerlinNoise() {
		this->gen_perm(perm2,19,123);
		this->gen_perm(perm3, 127, 321);
	}


	//
	inline float interpolate2(const float & ratio) const {
		return 3.0f * ratio * ratio - 2.0f *ratio*ratio*ratio;
	}


	inline void get_direction_vec(const Vec3 & p, Vec3 & pos, Vec3 & l_vec) const {
		for (int i = 0; i < 3; i++) 
			if (p[i] >= 0.f){
				pos[i] = (int)p[i]; 
				l_vec[i] = p[i] - pos[i];
			}	
			else {
				pos[i] = (int)p[i] - 1;
				l_vec[i] = p[i] - pos[i];
			}
	}

	inline Vec3 get_gradient(int i, int j, int k) const {
		auto idx = perm[i & 255] ^ perm2[j & 255] ^ perm3[k & 255];
		Vec3 result = { (float)perm[idx],(float)perm2[idx],(float)perm2[idx] };
		return result;
	}

	float noise(const Vec3 &p) const {
		//trilinear interpolation
		Vec3 pos;
		Vec3 l_vec;
		get_direction_vec(p, pos, l_vec);

		float tempf[3];
		for (int i = 0; i < 3; i++) {
			tempf[i] = interpolate2(l_vec.e[i]);
		}
		Vec3 ratio{tempf[0],tempf[1] ,tempf[2] };
		Vec3 ratio2 = 1.0f - ratio ;

		float sum = 0.0f;

		for (int i=0;i<2;i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					auto ijk = Vec3(i, j, k);
					Vec3 pos2 = pos + ijk ;

					Vec3 g = get_gradient(pos2.x(), pos2.y(), pos2.z());
					// length and direction
					Vec3 l_vec2 = l_vec - ijk;
					
					float temp1 = g.dot(l_vec2);
					float temp2 = (i*ratio[0] + (1 - i) * ratio2[0]) *
						(j*ratio[1] + (1 - j) * ratio2[1]) *
						(k*ratio[2] + (1 - k) * ratio2[2]);

					//cout << " ratio2 " << ratio2 << endl;
					//cout <<" l_vec " << l_vec << "   noise g   " << g << "  temp2 "<< temp2 << endl; //debug
					
					sum += temp1 * temp2;
					// get gradient value,
					// interpolate against that value
					
				}

		// normalize
		sum += 128;
		sum /= 255.0f;
		return sum;
	}

	float turb(const Vec3 &p, int depth = 7) const {
		float sum = 0.f;
		float weight = 1.0f;
		auto temp_p = p;
		float acc_weight = 0.0f;
		for (int i = 0; i < depth; i++) {
			acc_weight += weight;
			sum += weight * noise(temp_p);
			temp_p *= 2;
			weight *= 0.5;
		}
		sum /= acc_weight;
		return sum;
	}

};

