
#ifndef _MATH_VEC3_H
#define _MATH_VEC3_H

#include <emmintrin.h>
#include <iostream>
#pragma warning(error: 4714)
class vec2;

class vec3
{
public:
	__forceinline vec3() : mm{_mm_setzero_ps()} {};
	__forceinline vec3(const vec3& v) : mm{v.mm} {};
	__forceinline vec3(float x1, float y1, float z1) : x{x1}, y{y1}, z{z1} {}

	//vec3(vec2 v2, float z1);

	
	__forceinline explicit vec3(__m128 _mm) : mm{_mm} {}
	union {
		struct {float x,y,z;};
		__m128 mm;
	};

	static vec3 one, left, right, up, down, forwards, backwards;

	static vec3 cross(const vec3& a, const vec3& b);

	static float dot(const vec3& a, const vec3& b);

	// largest component
	static float largest(vec3 v);
	// smallest component
	static float smallest(vec3 v);

	float magnitude() const;
	vec3 normalized() const;

	friend vec3 operator*(const float& f, const vec3& v);

	bool operator!=(const vec3& v);

	vec3& operator*=(const vec3& v1);
	//vec3& operator/=(const vec3& v1);
	vec3& operator+=(const vec3& v1);
	vec3& operator-=(const vec3& v1);

	vec3& operator*=(const float& i);
	//vec3& operator/=(const float& i);
	vec3& operator+=(const float& i);
	vec3& operator-=(const float& i);

	// component wise max
	static inline vec3 max( vec3 a, vec3 b ) { return vec3(_mm_max_ps( a.mm, b.mm )); }
	// component wise min
	static inline vec3 min( vec3 a, vec3 b ) { return vec3(_mm_min_ps( a.mm, b.mm )); }

	// components wise absolute value
	static inline vec3 abs( vec3 v ) {
		__m128i minus1 = _mm_set1_epi32(-1);
  		__m128 abs_mask = _mm_castsi128_ps(_mm_srli_epi32(minus1, 1));
		return vec3(_mm_and_ps(abs_mask, v.mm));
	}

};

std::ostream& operator<<(std::ostream& os, const vec3& v);

inline vec3 operator*(vec3 a, vec3 b)
{
	return vec3(_mm_mul_ps(a.mm, b.mm));
}
//inline vec3 operator/(vec3 a, vec3 b)
//{
//	__m128 mask = _mm_castsi128_ps(_mm_set_epi32 (0xFFFF, 0, 0, 0));
//	__m128 c = _mm_and_ps( b.mm, mask );
//	return vec3(_mm_div_ps(a.mm, c));
//}
inline vec3 operator+(vec3 a, vec3 b)
{
	return vec3(_mm_add_ps(a.mm, b.mm));
}
inline vec3 operator-(vec3 a, vec3 b)
{
	return vec3(_mm_sub_ps(a.mm, b.mm));
}

inline vec3 operator*(vec3 v, const float& i)
{
	v *= i;
	return v;
}
//inline vec3 operator/(vec3 v, const float& i)
//{
//	v /= i;
//	return v;
//}
inline vec3 operator+(vec3 v, const float& i)
{
	v += i;
	return v;
}
inline vec3 operator-(vec3 v, const float& i)
{
	v -= i;
	return v;
}


// compare less than, 
// true if all components of a are less than b
inline bool vec3_cmplt( vec3 a, vec3 b ) {

}

// compare greater than
// returns true if all components of a are greater than b
inline bool vec3_cmpgt( vec3 a, vec3 b ) {

}

#endif // !VEC3