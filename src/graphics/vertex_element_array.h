#ifndef _GRAPHICS_VERTEX_ELEMENT_ARRAY_H
#define _GRAPHICS_VERTEX_ELEMENT_ARRAY_H

#include "vertex_array.h"
#include "element_buffer.h"
#include "vertex_buffer.h"

class VertexElementArray : public VertexArray {
public:
	void bindAll() {
		bind();
		elementBuffer.bind();
		vertexBuffer.bind();
	}
	void unbindAll() {
		unbind();
		elementBuffer.unbind();
		vertexBuffer.unbind();
	}
	template <typename T>
	void bufferData(std::vector<T>& vertices, GLenum vertexUsage, std::vector<unsigned int>& elements, GLenum elementUsage) {
		vertexBuffer.bufferData<T>(vertices, vertexUsage);
		elementBuffer.bufferData(elements, elementUsage);
	}
	static VertexElementArray Generate() {
		VertexElementArray result = VertexArray::Generate();
		result.mElementBuffer = ElementBuffer::Generate();
		result.mVertexBuffer = VertexBuffer::Generate();
		return result;
	}
	ElementBuffer mElementBuffer;
	VertexBuffer mVertexBuffer;
};


#endif