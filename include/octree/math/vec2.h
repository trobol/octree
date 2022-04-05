#ifndef _MATH_VEC2_H
#define _MATH_VEC2_H

struct vec2 {
	vec2() : x{ 0 }, y{ 0 }{}
	vec2(float _x, float _y) : x{ _x }, y{ _y } {}
	float x, y;
};



#endif