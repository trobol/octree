
#include <octree/drawables/axis_drawable.h>
#include <octree/graphics/vertex_attribute_distriptor.h>
#include <octree/graphics/buffer.h>


void AxisDrawable::Initialize() {
	m_vertexArray = VertexArray::Generate();
	Buffer vertexBuffer = Buffer::Generate();

	m_vertexArray.bind();
	vertexBuffer.bind(GL_ARRAY_BUFFER);

	VertexAttributeDiscriptor discriptor;
	discriptor.add(3, GL_FLOAT); // vertex position attribute
	discriptor.add(3, GL_FLOAT); // instance position attribute
	discriptor.add(3, GL_FLOAT); // color attribute
	discriptor.add(1, GL_FLOAT); // size
	discriptor.apply();

	float vertexData[60] = {
		// x, y, z  r, g, b
		0, 0, 0,	0, 0, 0,	1, 0, 0,	5,
		1, 0, 0,	0, 0, 0,	1, 0, 0,	5,
		0, 0, 0,	0, 0, 0,	0, 1, 0,	5,
		0, 1, 0,	0, 0, 0,	0, 1, 0,	5,
		0, 0, 0,	0, 0, 0,	0, 0, 1,	5,
		0, 0, 1,	0, 0, 0,	0, 0, 1,	5,
	};
	
	vertexBuffer.bufferArray(vertexData, 60, GL_STATIC_DRAW);
	m_vertexArray.unbind();
	vertexBuffer.unbind();
}


void AxisDrawable::Draw() {
	glLineWidth(10);
	m_vertexArray.bind();
	glDrawArrays(GL_LINES, 0, 6);
}


