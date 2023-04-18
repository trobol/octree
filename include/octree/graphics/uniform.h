#ifndef _GRAPHICS_UNIFORM_H
#define _GRAPHICS_UNIFORM_H

#include "../math/mat4.h"
#include <octree/graphics/shader.h>
#include "../math/vec2.h"

struct UniformInfo {
	GLint location;
	UniformInfo* next;
	const char name[];
};
template <typename T>
class Uniform
{
public:
	void set(const T& value, GLboolean transpose = GL_FALSE);
	Uniform& operator=(const T& value)
	{
		set(value);
		return *this;
	}
private:
	Uniform(UniformInfo* _info) : info{_info} {}
protected:
	UniformInfo* info;
friend class Shader;
};

/*
TEMPLATE SPECIALIZATIONS
*/

using UniformMatrix4f = Uniform<mat4>;

template<>
inline void Uniform<mat4>::set(const mat4& value, GLboolean transpose) {
	glUniformMatrix4fv(info->location, 1, GL_FALSE, value.matrix);
}


template <>
inline void Uniform<int>::set(const int& value, GLboolean transpose)
{
	glUniform1i(info->location, value);
}

template <>
inline void Uniform<vec2>::set(const vec2& value, GLboolean transpose) {
	glUniform2f(info->location, value.x, value.y);
}

template <>
inline void Uniform<float>::set(const float& value, GLboolean transpose) {
	glUniform1f(info->location, value);
}

#endif