#ifndef _DRAWABLE_LEAFMESH_H
#define  _DRAWABLE_LEAFMESH_H

#include "drawable.h"

class LeafMeshDrawable : public Drawable {

public:
	virtual void Initialize() override;
	virtual void Draw() override;
};

#endif