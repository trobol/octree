#include <systems/window.h>
#include <iostream>


#define GL_LITE_IMPLEMENTATION
#include <graphics/gl_lite.h>


static void error_callback(int error, const char* description)
{
	std::cout << "Error: " << description << std::endl;
}

void Window::windowSizeCallback(GLFWwindow* window, int width, int height) {
	Window& instance = getInstance();
	instance.mAspectRatio = (float)width / height;
	instance.mWidth = width;
	instance.mHeight = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Window::open() {

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	mWindow = glfwCreateWindow(mWidth, mHeight, "Octree render", NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowSizeCallback(mWindow, Window::windowSizeCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

	mAspectRatio = mWidth / mHeight;

	glfwMakeContextCurrent(mWindow);
	gl_lite_init();
	glfwSwapInterval(1);


}


void Window::open(int width, int height) {
	mWidth = width;
	mHeight = height;
	open();
}


void Window::close() {
	glfwDestroyWindow(mWindow);
}