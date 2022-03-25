#include <octree/drawables/voxel_mesh.h>
#include "../defines.h"


void VoxelMeshDrawable::Initialize() {

	m_instanceBuffer = Buffer::Generate();
	//branchInstanceBuffer.bind(GL_ARRAY_BUFFER);
	//branchInstanceBuffer.bufferVector(instances, GL_STATIC_DRAW);
	//branchInstanceBuffer.unbind();

	m_vertexArray = VertexArray::Generate();
	Buffer vertexBuffer = Buffer::Generate();
	Buffer elementBuffer = Buffer::Generate();

	m_vertexArray.bind();
	vertexBuffer.bind(GL_ARRAY_BUFFER);
	elementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);


	vertexBuffer.bufferArray(CUBE_POINTS, ELEMENTS_PER_CUBE, GL_STATIC_DRAW);
 
	unsigned int indices_count = m_outline ? INDICES_PER_BRANCH : INDICES_PER_LEAF;
	const unsigned int* indices_ptr = m_outline ? BRANCH_INDICES : LEAF_INDICES;
	elementBuffer.bufferArray(indices_ptr, indices_count, GL_STATIC_DRAW);

	VertexAttributeDiscriptor discriptor;
	discriptor.add(3, GL_FLOAT); // position attribute
	//discriptor.add(3, GL_FLOAT); moved to instance
	discriptor.apply();

	//instanced attributes
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	m_instanceBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
	m_instanceBuffer.unbind();
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

	m_vertexArray.unbind();
	vertexBuffer.unbind();
	elementBuffer.unbind();
}

void VoxelMeshDrawable::SetInstances(std::vector<Cube>& instances) {
	// load leaf instance buffer
	m_instanceBuffer.bind(GL_ARRAY_BUFFER);
	m_instanceBuffer.bufferVector(instances, GL_STATIC_DRAW);
	m_instanceBuffer.unbind();
	m_instanceCount = instances.size();
}

void VoxelMeshDrawable::Draw() {
	m_vertexArray.bind();
	if (m_outline) {
		glLineWidth(1);
		glDrawElementsInstanced(GL_LINES, INDICES_PER_BRANCH, GL_UNSIGNED_INT, 0, m_instanceCount);
	} else {
		glDrawElementsInstanced(GL_TRIANGLES, INDICES_PER_LEAF, GL_UNSIGNED_INT, 0, m_instanceCount);
	}
}