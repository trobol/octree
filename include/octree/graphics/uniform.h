#ifndef _GRAPHICS_UNIFORM_H
#define _GRAPHICS_UNIFORM_H

#include "../math/mat4.h"
#include <octree/graphics/shader.h>

template <typename T>
class Uniform
{
public:
	Uniform(Shader& shader, const char* name)
	{
		mLocation = glGetUniformLocation((GLuint)shader, name);
	}
	void set(const T& value, GLboolean transpose = GL_FALSE);
	Uniform& operator=(const T& value)
	{
		set(value);
		return *this;
	}

protected:
	GLint mLocation;
};

/*
TEMPLATE SPECIALIZATIONS
*/

using UniformMatrix4f = Uniform<mat4>;

template<>
inline void Uniform<mat4>::set(const mat4& value, GLboolean transpose) {
	glUniformMatrix4fv(mLocation, 1, GL_FALSE, value.matrix);
}



template <>
inline void Uniform<float>::set(const float& value, GLboolean transpose)
{
	glUniformMatrix4fv(mLocation, 1, GL_FALSE, &value);
}


template <>
inline void Uniform<int>::set(const int& value, GLboolean transpose)
{
	glUniform1i(mLocation, value);
}

#endif