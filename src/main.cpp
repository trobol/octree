#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "octree.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include <math.h>
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

Camera camera;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_W)
		camera.mTransform.position.z -= 0.2;
	if (key == GLFW_KEY_S)
		camera.mTransform.position.z += 0.2;
	if (key == GLFW_KEY_D)
		camera.mTransform.rotateAroundOrigin(0.2);
	if (key == GLFW_KEY_A)
		camera.mTransform.rotateAroundOrigin(-0.2);
}

Shader shader;

void window_size_callback(GLFWwindow *window, int width, int height)
{
	camera.mAspectRatio = (float)width / height;
	std::cout << camera.mAspectRatio << std::endl;
}

int main(void)
{
	Octree tree(4);
	tree.setNode(1, 1, 1);
	tree.setNode(2, 2, 2);
	tree.setNode(2, 2, 4);

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

	glfwSetWindowSizeCallback(window, window_size_callback);
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

	std::cout << std::endl;
	std::cout << elements.size() << std::endl;
	std::cout << indices.size() << std::endl;

	glUseProgram(shader);

	camera.mTransform.scale = vec3(1, 1, 1);
	camera.mTransform.position = vec3(0, 0, 10.);

	int projMatrix = glGetUniformLocation(shader, "projMatrix");
	int camMatrix = glGetUniformLocation(shader, "camMatrix");

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vec3 pos = camera.mTransform.position;
		glUniformMatrix4fv(projMatrix, 1, GL_FALSE, camera.getProjMatrix());
		camera.mTransform.rotation = Quaternion::AxisAngle(vec3::up, atan2(-pos.x, pos.z));
		glUniformMatrix4fv(camMatrix, 1, GL_FALSE, camera.getTransformMatrix());

		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
