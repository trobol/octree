#ifndef _GRAPHICS_VERTEX_ATTRIBUTE_DISTRIPTOR_H
#define _GRAPHICS_VERTEX_ATTRIBUTE_DISTRIPTOR_H

#include "../gl_lite.h"

#include <vector>

class VertexAttributeDiscriptor {
public:
	struct VertexAttribute {
		GLint size;
		GLenum type;
		GLboolean normalized;
		size_t typeSize; //size of the attribute in bytes
	};
	void add(GLuint size, GLenum type, GLboolean normalized) {
		size_t typeSize;
		switch (type)
		{
		case GL_BYTE: typeSize = 1; break;
		case GL_UNSIGNED_BYTE: typeSize = 1; break;
		case GL_SHORT: typeSize = 2; break;
		case GL_UNSIGNED_SHORT: typeSize = 2; break;
		case GL_INT: typeSize = 4; break;
		case GL_UNSIGNED_INT: typeSize = 4; break;
		case GL_FLOAT: typeSize = 4; break;
		case GL_DOUBLE: typeSize = 8; break;
		default:
			throw "invalid gl type";
		}
		mStride += typeSize * size;
		mAttributes.push_back(VertexAttribute(size, type, normalized, typeSize));
	}
	void finish() {
		GLsizei ptr = 0;
		for (int i = 0; i < mAttributes.size(); i++) {
			VertexAttribute attr = mAttributes[i];

			glVertexAttribPointer(i, attr.size, attr.type, attr.normalized, mStride, ptr);
			glEnableVertexAttribArray(i);
			ptr += attr.typeSize * attr.size;
		}
	}
private:
	std::vector<VertexAttribute> mAttributes;
	GLsizei mStride = 0;
};

#endif