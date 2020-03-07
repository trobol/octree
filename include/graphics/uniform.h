#ifndef _GRAPHICS_UNIFORM_H
#define _GRAPHICS_UNIFORM_H

#include "../math/mat4.h"
#include <graphics/shader.h>

template <typename T>
class Uniform
{
public:
	Uniform(Shader& shader, const char* name)
	{
		mLocation = glGetUniformLocation((GLuint)shader, name);
	}
	void set(const T& value, GLboolean transpose = GL_FALSE) {}
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
void Uniform<mat4>::set(const mat4& value, GLboolean transpose) {
	glUniformMatrix4fv(mLocation, 1, GL_FALSE, value.matrix);
}



template <>
void Uniform<float>::set(const float& value, GLboolean transpose)
{
	glUniformMatrix4fv(mLocation, 1, GL_FALSE, &value);
}


#endif