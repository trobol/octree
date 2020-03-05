#ifndef _GRAPHICS_UNIFORM_H
#define _GRAPHICS_UNIFORM_H

#include "../math/mat4.h"

class Shader;

template <typename T>
class Uniform
{
public:
	Uniform(Shader &shader, const char *name)
	{
		mLocation = glGetUniformLocation((GLuint)shader, name);
	}
	virtual void set(const T &value, GLboolean transpose = GL_FALSE) = 0;
	Uniform &operator=(const T &value)
	{
		set(value);
		return *this;
	}

protected:
	GLint mLocation;
};

class UniformMatrix4f : public Uniform<mat4>
{
public:
	using Uniform::Uniform;
	using Uniform::operator=;
	void set(const mat4 &value, GLboolean transpose = GL_FALSE) override
	{
		glUniformMatrix4fv(mLocation, 1, GL_FALSE, value.matrix);
	}
};

class Uniform1f : public Uniform<float>
{
public:
	using Uniform::operator=;
	using Uniform::Uniform;
	void set(const float &value, GLboolean transpose = GL_FALSE) override
	{
		glUniformMatrix4fv(mLocation, 1, GL_FALSE, &value);
	}
};

#endif