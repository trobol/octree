#ifndef _AXISDRAWABLE_DRAWABLE_H
#define _AXISDRAWABLE_DRAWABLE_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>

class AxisDrawable : public Drawable {

public:
	AxisDrawable() : Drawable{"axis"} {}

	virtual void Draw() override;
	virtual void Initialize() override;

private:
	VertexArray m_vertexArray;
};


#endif