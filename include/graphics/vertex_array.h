#ifndef _GRAPHICS_VERTEX_ARRAY_H
#define _GRAPHICS_VERTEX_ARRAY_H

#include "gl_lite.h"


class VertexArray {
public:

	static VertexArray Generate() {
		VertexArray result;
		glGenVertexArrays(1, &result.mId);
		return result;
	}
	void bind() {
		glBindVertexArray(mId);
	}
	void unbind() {
		glBindVertexArray(0);
	}
private:
	GLuint mId;
};

#endif