#ifndef _GRAPHICS_VERTEX_BUFFER_H
#define _GRAPHICS_VERTEX_BUFFER_H

#include "buffer.h"

class VertexBuffer : public Buffer {
public:
	void bind() {
		bind(GL_ELEMENT_ARRAY_BUFFER, mId);
	}
};

#endif