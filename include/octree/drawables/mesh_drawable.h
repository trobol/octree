#ifndef _MESHDRAWABLE_DRAWABLE_H
#define _MESHDRAWABLE_DRAWABLE_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>

class MeshDrawable : public Drawable {

public:
	MeshDrawable(std::string name) : Drawable{name} {}

	virtual void Draw() override {};
	virtual void Initialize() override {};
	
private:
	VertexArray m_vertexArray;
};


#endif