#ifndef _COMPUTEDRAWABLE_DRAWABLE_H
#define _COMPUTEDRAWABLE_DRAWABLE_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>

class ComputeDrawable : public Drawable {

public:
	ComputeDrawable(std::string name) : Drawable{name} {}

	virtual void Draw() override;
	virtual void Initialize() override;
	
private:
	VertexArray m_vertexArray;
};


#endif