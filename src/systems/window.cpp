#include <octree/systems/window.h>
#include <iostream>


#define GL_LITE_IMPLEMENTATION
#include <octree/graphics/gl_lite.h>


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

void Window::startup() {

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	mWindow = glfwCreateWindow(mWidth, mHeight, "Octree render", NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	
	
	glfwSetWindowSizeCallback(mWindow, Window::windowSizeCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

	mAspectRatio = (float)mWidth / (float)mHeight;

	glfwMakeContextCurrent(mWindow);
	gl_lite_init();
	glfwSwapInterval(1);


}


void Window::setSize(int width, int height) {
	mWidth = width;
	mHeight = height;
	mAspectRatio = (float)width / (float)height;
}


void Window::shutdown() {
	glfwDestroyWindow(mWindow);
}