#ifndef _DRAWABLE_VOXELMESH_H
#define  _DRAWABLE_VOXELMESH_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>
#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>
#include <octree/graphics/uniform.h>
#include <octree/octree_chunk.h>

class VoxelMeshDrawable : public Drawable {

public:
	VoxelMeshDrawable(std::string name, bool outline) : Drawable{name}, m_outline{outline} {}
	virtual void Initialize() override;
	virtual void Draw() override;

	void SetInstances(std::vector<Cube>& cubes);
private:
	bool m_outline;
	Buffer m_instanceBuffer;
	VertexArray m_vertexArray;

	uint32_t m_instanceCount;
};

#endif