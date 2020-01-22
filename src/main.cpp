#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "octree.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "shader.h"

#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

Point vertices[3] =
	{
		{{-0.6f, -0.4f, 0.f}, {1.f, 0.f, 0.f}},
		{{0.6f, -0.4f, 0.f}, {0.f, 1.f, 0.f}},
		{{0.f, 0.6f, 0.f}, {0.f, 0.f, 1.f}}};
static const int indies[] = {
	0, 1, 2};
static void
error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Shader shader;

int main(void)
{
	Octree tree(4);
	tree.setNode(1, 1, 1);
	tree.setNode(2, 2, 2);

	printNode(tree.mRootNode);
	GLFWwindow *window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gl_lite_init();
	glfwSwapInterval(1);

	// NOTE: OpenGL error checks have been omitted for brevity

	shader = Shader::Load("shaders/shader.vert", "shaders/shader.frag");
	glUseProgram(shader);

	std::vector<Point> elements;
	std::vector<int> indices;

	tree.drawNodes(elements, indices);

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(Point), elements.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	for (int i = 0; i < indices.size(); i++)
	{
		std::cout << indices[i] << '\n';
	}
	std::cout << std::endl;
	std::cout << elements.size() << std::endl;
	std::cout << indices.size() << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
