#ifndef _GRAPHICS_BUFFER_H
#define _GRAPHICS_BUFFER_H

#include "../gl_lite.h"

class Buffer {
public:
	static Buffer Generate() {
		Buffer result;
		glGenBuffers(1, &result.mId);
		return result;
	}
	operator GLuint() { return mId; };
	void bind(GLenum target) { glBindBuffer(target, mId); };
private:
	unsigned int mId;
};

#endif


