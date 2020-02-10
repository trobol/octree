#ifndef _GRAPHICS_BUFFER_H
#define _GRAPHICS_BUFFER_H

#include "gl_lite.h"
#include <vector>


class Buffer {
public:
	static Buffer Generate() {
		Buffer result;
		glGenBuffers(1, &result.mId);
		return result;
	}
	operator GLuint() { return mId; };
	void bind(GLenum target) { glBindBuffer(target, mId); };
	void unbind() {
		if (bindingPoint) {
			glBindBuffer(bindingPoint, 0);
		}
	}

	template <typename T>
	void bufferData(std::vector<T>& data, GLenum usage) {
		if (bindingPoint) {
			glBufferData(bindingPoint, data.size() * sizeof(T), data.data(), usage);
		}
	}

private:
	GLuint mId;
	GLenum bindingPoint = NULL;
};

#endif


