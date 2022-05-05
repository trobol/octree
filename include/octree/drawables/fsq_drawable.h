#ifndef _FSQDRAWABLE_DRAWABLE_H
#define _FSQDRAWABLE_DRAWABLE_H

#include "drawable.h"
#include <octree/graphics/vertex_array.h>
#include <octree/graphics/shader.h>

#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>

class FSQDrawable : public Drawable {

public:
	FSQDrawable(std::string name) : Drawable{name} {}

	virtual void Draw() override;
	virtual void Initialize() override;
	
	void SetSize(unsigned int width, unsigned int height);
	GLuint GetTexture() { return m_texture; }

private:

	GLuint m_texture;
	Shader m_shader;
};


#endif