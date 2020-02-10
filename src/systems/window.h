#ifndef _SYSTEMS_WINDOW_H
#define _SYSTEMS_WINDOW_H

#include <Windows.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

class Window {
public:
	static Window& getInstance() {
		static Window instance;
		return instance;
	}
	void open();
	void open(int width, int height);
	void close();

	float getAspectRatio() { return mAspectRatio; };

	void setKeyCallback(GLFWkeyfun key_callback) {
		glfwSetKeyCallback(mWindow, key_callback);
	}
	void setScrollCallback(GLFWscrollfun scroll_callback) {
		glfwSetScrollCallback(mWindow, scroll_callback);
	}
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }


	bool shouldClose() {
		return glfwWindowShouldClose(mWindow);
	}

	void swapBuffers() {
		glfwSwapBuffers(mWindow);
	}
private:
	GLFWwindow* mWindow;
	float mAspectRatio;
	int mWidth = 640,
		mHeight = 480;

	static void windowSizeCallback(GLFWwindow* window, int width, int height);

};

#endif
