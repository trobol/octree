#ifndef _SYSTEMS_WINDOW_H
#define _SYSTEMS_WINDOW_H

#include <core/isystem.h>

#include <graphics/gl_lite.h>

#include <GLFW/glfw3.h>
#include "system.h"


class Window : public System<Window> {
public:
	
	void startup();
	
	void shutdown();

	float getAspectRatio() { return mAspectRatio; };
	void setSize(int width, int height);

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
	void Update(float dt) {

	}
	~Window() {

	}
private:
	GLFWwindow* mWindow = nullptr;
	float mAspectRatio = 0;
	int mWidth = 1500,
		mHeight = 480;

	static void windowSizeCallback(GLFWwindow* window, int width, int height);

};

#endif
