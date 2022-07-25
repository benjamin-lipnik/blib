#ifndef BUITL_H
#define BUITL_H

#include "bint.h"

#define UTIL_LIST \
	/* type, name */ \
	BTL_MAX(int,   maxi)  \
	BTL_MIN(int,   mini)  \
	BTL_MAX(float, maxf)  \
	BTL_MIN(float, minf)  \
	BTL_MAP(float, mapf)  \
	BTL_MAP(double, mapd) \
	/* fertik */

#define BTL_FCN(type, name) static inline type btl_##name

#define BTL_MIN(type, name) \
	BTL_FCN(type,name)(type a, type b) { \
		return (a<b)?a:b; \
	}
#define BTL_MAX(type, name) \
	BTL_FCN(type,name)(type a, type b) { \
		return (a>b)?a:b; \
	}
#define BTL_MAP(type, name) \
	BTL_FCN(type, name)(type x, type from_min, type from_max, type to_min, type to_max) { \
		float k=(to_max-to_min)/(from_max - from_min); \
		return to_min + k*(x-from_min); \
	}

UTIL_LIST

void btl_sleep(int sleep_ms);
Mat2 btl_mat2_multiply(Mat2 a, Mat2 b);
Mat2 btl_mat2_rotacija(float fi);
Mat2 btl_mat2_rotiraj(Mat2 m, float fi);
Mat2 btl_mat2_id();
Vec2f btl_mat2_preslikava(Mat2 m, Vec2f v);

#ifdef BUTIL_IMPLEMENTATION

#include <math.h>

Mat2 btl_mat2_id() {
	return (Mat2){
		1, 0,
		0, 1
	};
}
Mat2 btl_mat2_multiply(Mat2 a, Mat2 b) {
	return (Mat2){
		(a.a * b.a) + (a.b * b.c), 
		(a.a * b.b) + (a.b * b.d),
		(a.c * b.a) + (a.d * b.c),
		(a.c * b.b) + (a.d * b.d)
	};
}
Mat2 btl_mat2_rotacija(float fi) {
	float cos_fi = cos(fi);
	float sin_fi = sin(fi);
	return (Mat2){
		cos_fi, -sin_fi,
		sin_fi,  cos_fi
	};
}
Mat2 btl_mat2_rotiraj(Mat2 m, float fi) {
	return btl_mat2_multiply(m, btl_mat2_rotacija(fi));
}

Vec2f btl_mat2_preslikava(Mat2 m, Vec2f v) {
	return (Vec2f){
		(m.a * v.x) + (m.b * v.y),
		(m.c * v.x) + (m.d * v.y)
	};
}

#ifdef WINDOWS
	#include <windows.h>
#else
	#include <unistd.h>
#endif

void btl_sleep(int sleep_ms) {
#ifdef WINDOWS
    Sleep(sleep_ms);
#else
    usleep(sleep_ms * 1000);
#endif
}

#endif
#endif
