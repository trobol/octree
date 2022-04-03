#ifndef _DRAWABLE_BRANCHMESH_H
#define  _DRAWABLE_BRANCHMESH_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>
#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>
#include <octree/graphics/uniform.h>

class BranchMeshDrawable : public Drawable {

public:
	BranchMeshDrawable() : Drawable{"branch_mesh"} {}
	virtual void Initialize() override;
	virtual void Draw() override;

	void SetInstances(std::vector<Cube>& instances);
private:
	Buffer m_instanceBuffer;
	VertexArray m_vertexArray;

	uint32_t m_instanceCount;
};

#endif