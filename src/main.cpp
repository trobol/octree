#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <Windows.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "octree.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "graphics/shader.h"

#include <math.h>

#include "files/filesystem.h"

#include <time.h> /* time */

#include "graphics/buffer.h"
#include "graphics/vertex_attribute_distriptor.h"

#include "graphics/uniform.h"
#include "graphics/vertex_array.h"

#include "systems/window.h"
#include "camera.h"


const std::string ASSET_PATH_STR = ASSET_PATH;

Camera camera;


vec3 center = vec3(0, 0, 0);
bool drawBranches = true;
bool drawLeafs = true;

float speed = 0.2f;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	vec3 forward = (center - camera.mTransform.position).normalized();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_W)
		camera.mTransform.position += forward * speed;

	if (key == GLFW_KEY_S)
		camera.mTransform.position -= forward * speed;

	if (key == GLFW_KEY_SPACE)
		camera.mTransform.position.y += speed;
	if (key == GLFW_KEY_LEFT_SHIFT)
		camera.mTransform.position.y -= speed;
	if (key == GLFW_KEY_D)
		camera.mTransform.rotateAroundDistance(center, speed);

	if (key == GLFW_KEY_A)
		camera.mTransform.rotateAroundDistance(center, -speed);

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		drawBranches = !drawBranches;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		drawLeafs = !drawLeafs;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	speed = max(0, speed + yoffset * 0.05);
}
Shader shader;

Window& window = Window::getInstance();

int main(void)
{
	srand(time(NULL));
	VoxFile file;
	//file.load("assets/teapot.vox");
	std::string filepath = filesystem::fileSelect(ASSET_PATH_STR + "/models/", ".vox");
	file.load(filepath);
	//file.load("../../assets/box.vox");
	Octree tree(4);
	tree.loadModel(file);


	window.open();
	// NOTE: OpenGL error checks have been omitted for brevity
	window.setKeyCallback(key_callback);
	window.setScrollCallback(scroll_callback);

	shader = Shader::Load(ASSET_PATH_STR + "/shaders/shader.vert", ASSET_PATH_STR + "/shaders/shader.frag");

	glUseProgram(shader);

	std::vector<Point> elements;
	std::vector<int> indices;

	std::vector<int> leafIndices;
	std::vector<Point> leafElements;

	tree.drawNodes(elements, indices, leafElements, leafIndices);


	VertexAttributeDiscriptor discriptor;

	// position attribute
	discriptor.add(3, GL_FLOAT);
	// color attribute
	discriptor.add(3, GL_FLOAT);


	VertexArray branchVertexArray = VertexArray::Generate();
	Buffer branchVertexBuffer = Buffer::Generate();
	Buffer branchElementBuffer = Buffer::Generate();




	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	branchVertexArray.bind();
	branchVertexBuffer.bind(GL_ARRAY_BUFFER);
	branchElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);


	discriptor.apply();


	glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(Point), elements.data(), GL_DYNAMIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);


	branchVertexArray.unbind();
	branchVertexBuffer.unbind();
	branchElementBuffer.unbind();


	VertexArray leafVertexArray = VertexArray::Generate();
	Buffer leafVertexBuffer = Buffer::Generate();
	Buffer leafElementBuffer = Buffer::Generate();

	leafVertexArray.bind();
	leafVertexBuffer.bind(GL_ARRAY_BUFFER);
	leafElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

	discriptor.apply();

	glBufferData(GL_ARRAY_BUFFER, leafElements.size() * sizeof(Point), leafElements.data(), GL_DYNAMIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, leafIndices.size() * sizeof(int), leafIndices.data(), GL_DYNAMIC_DRAW);

	leafVertexArray.unbind();
	leafVertexBuffer.unbind();
	leafElementBuffer.unbind();


	/*
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
	*/
	shader.use();


	camera.mTransform.scale = vec3(1, 1, 1);
	camera.mTransform.position = vec3(0, 0, 10.);

	center = file.getSize().toFloat() / 2;

	UniformMatrix4f projMatrix(shader, "projMatrix");
	UniformMatrix4f camMatrix(shader, "camMatrix");

	glEnable(GL_DEPTH_TEST);
	while (!window.shouldClose())
	{


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projMatrix = camera.getProjMatrix();
		camMatrix = mat4::lookAt(camera.mTransform.position, center, vec3::up);

		//draw branches

		if (drawBranches)
		{
			branchVertexArray.bind();

			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		if (drawLeafs)
		{
			leafVertexArray.bind();

			glDrawElements(GL_TRIANGLES, leafIndices.size(), GL_UNSIGNED_INT, 0);
		}
		window.swapBuffers();
		glfwPollEvents();
	}

	window.close();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
