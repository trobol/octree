#include <octree/drawables/fsq_drawable.h>


static GLuint FSQ_VA = 0;


float quadVerts[] = { -1.0f, -1.0f, 0.0f, 0.0f,
                      -1.0f, 1.0f, 0.0f, 1.0f,
					  1.0f, -1.0f, 1.0f, 0.0f,
					  1.0f, 1.0f, 1.0f, 1.0f };

void loadQUAD() {
	glGenVertexArrays(1, &FSQ_VA);
	Buffer quadVertexBuffer = Buffer::Generate();
	glBindVertexArray(FSQ_VA);
	quadVertexBuffer.bind(GL_ARRAY_BUFFER);

	quadVertexBuffer.bufferArray(quadVerts, 16, GL_STATIC_DRAW);

	VertexAttributeDiscriptor quadDescriptor;
	quadDescriptor.add(2, GL_FLOAT);
	quadDescriptor.add(2, GL_FLOAT);
	quadDescriptor.apply();

	glBindVertexArray(0);
	quadVertexBuffer.unbind();
}

void FSQDrawable::Initialize() {
	if (FSQ_VA == 0) {
		loadQUAD();
	}

}


void FSQDrawable::Draw() {
	m_shader.use();
	glBindVertexArray(FSQ_VA);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
	
void FSQDrawable::SetSize(unsigned int width, unsigned int height) {
	glTexImage2D (m_texture, 0,  GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
}