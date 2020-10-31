#ifndef _GRAPHICS_ELEMENT_BUFFER_H
#define _GRAPHICS_ELEMENT_BUFFER_H

#include "buffer.h"

class ElementBuffer : public Buffer {
public:
	void bind() {
		bind(GL_ELEMENT_ARRAY_BUFFER, mId);
	}
	void bufferData(std::vector<GLuint>& elements, GLenum usage) {
		bufferData<GLuint>(elements, usage);
	}
};

#endif