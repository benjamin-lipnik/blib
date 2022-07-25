#ifndef BINT_H
#define BINT_H

#include <stdint.h>

#define VEC_TYPE_LIST \
	VEC2(int32_t, i) \
	VEC2(uint32_t, ui) \
	VEC2(float, f) \
	VEC3(int32_t, i) \
	VEC3(uint32_t, ui) \
	VEC3(float, f) \
	VEC4(int32_t, i) \
	VEC4(uint32_t, ui) \
	VEC4(float, f)

#define VEC2(type, identifier) \
	typedef struct { \
		type x,y; \
	}Vec2##identifier;

#define VEC3(type, identifier) \
	typedef struct { \
		type x,y,z; \
	}Vec3##identifier;
	
#define VEC4(type, identifier) \
	typedef struct { \
		type x,y,z,w; \
	}Vec4##identifier;

typedef struct {
	float a,b;
	float c,d;
}Mat2;

VEC_TYPE_LIST

#endif
