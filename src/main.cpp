
#include <Windows.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <octree/octree.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <octree/graphics/shader.h>
#include <octree/math/math.h>
#include <octree/math/vec3.h>


#include <octree/core/files/filesystem.h>

#include <time.h> /* time */

#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>

#include <octree/graphics/uniform.h>
#include <octree/graphics/vertex_array.h>

#include <octree/systems/window.h>
#include <octree/camera.h>
#include "defines.h"

const std::string ASSET_PATH_STR = ASSET_PATH;

Camera camera;

vec3 center = vec3(0, 0, 0);
bool drawBranches = true;
bool drawLeafs = true;

float speed = 1;

bool absolute_movement = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	vec3 forward = (center - camera.mTransform.position).normalized();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	// VISUALIZATION
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		drawBranches = !drawBranches;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		drawLeafs = !drawLeafs;


	// MOVEMENT
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



	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.mTransform.position = vec3(0, 0, 10);
		camera.mTransform.rotation = Quaternion::identity;
	}
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

	std::string filepath = filesystem::fileSelect(ASSET_PATH_STR + "/models/", ".vox");
	file.load(filepath);
	//file.load("../../assets/box.vox");
	Octree tree = Octree::loadModel(file);

	window.startup();
	// NOTE: OpenGL error checks have been omitted for brevity
	window.setKeyCallback(key_callback);
	window.setScrollCallback(scroll_callback);

	std::string vertPath = ASSET_PATH_STR + "/shaders/shader.vert";
	std::string fragPath = ASSET_PATH_STR + "/shaders/shader.frag";
	shader = Shader::Load(vertPath, fragPath);

	shader.use();


	std::vector<Cube> instances;
	std::vector<Cube> leafInstances;

	tree.drawNodes(instances, leafInstances);


	std::vector<GLint> branchIndices(instances.size() * INDICES_PER_BRANCH);
	std::vector<Point> branchElements(instances.size() * ELEMENTS_PER_CUBE);


	for (size_t i = 0; i < instances.size(); i++)
	{
		for (size_t j = 0; j < INDICES_PER_BRANCH; j++)
		{
			uint32_t offset = (i * INDICES_PER_BRANCH);
			branchIndices[j + offset] = BRANCH_INDICES[j] + offset;
		}
	}
	for (size_t i = 0; i < instances.size(); i++)
	{
		Cube c = instances[i];
		for (int j = 0; j < ELEMENTS_PER_CUBE; j++)
		{
			Point p;
			p.pos = (CUBE_POINTS[j] * c.size) + c.pos;
			p.color = c.color;
			branchElements.push_back(p);
		}
	}



	std::vector<GLint> leafIndices(leafInstances.size() * INDICES_PER_LEAF);
	std::vector<Point> leafElements;


	for (size_t i = 0; i < leafInstances.size(); i++)
	{
		for (size_t j = 0; j < INDICES_PER_LEAF; j++)
		{
			leafIndices[j + (i * INDICES_PER_LEAF)] = LEAF_INDICES[j] + (i * ELEMENTS_PER_CUBE);
		}
	}
	for (size_t i = 0; i < leafInstances.size(); i++)
	{
		Cube c = leafInstances[i];
		for (int j = 0; j < ELEMENTS_PER_CUBE; j++)
		{
			Point p;
			p.pos = (CUBE_POINTS[j]) + c.pos;
			p.color = c.color;
			leafElements.push_back(p);
		}
	}



	VertexArray branchVertexArray = VertexArray::Generate();
	Buffer branchVertexBuffer = Buffer::Generate();
	Buffer branchElementBuffer = Buffer::Generate();


	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	branchVertexArray.bind();
	branchVertexBuffer.bind(GL_ARRAY_BUFFER);
	branchElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

	branchVertexBuffer.bufferVector(branchElements, GL_DYNAMIC_DRAW);
	branchElementBuffer.bufferVector(branchIndices, GL_DYNAMIC_DRAW);


	VertexAttributeDiscriptor discriptor;
	discriptor.add(3, GL_FLOAT); // position attribute
	discriptor.add(3, GL_FLOAT);
	discriptor.apply();

	branchVertexArray.unbind();
	branchVertexBuffer.unbind();
	branchElementBuffer.unbind();

	VertexArray leafVertexArray = VertexArray::Generate();
	Buffer leafVertexBuffer = Buffer::Generate();
	Buffer leafElementBuffer = Buffer::Generate();

	leafVertexArray.bind();
	leafVertexBuffer.bind(GL_ARRAY_BUFFER);
	leafElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);



	leafVertexBuffer.bufferVector(leafElements, GL_DYNAMIC_DRAW);
	leafElementBuffer.bufferVector(leafIndices, GL_DYNAMIC_DRAW);

	discriptor.apply();

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

	center = vec3(32, 32, 32) / 2;

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

			glDrawElements(GL_LINES, branchIndices.size(), GL_UNSIGNED_INT, 0);
		}
		if (drawLeafs)
		{
			leafVertexArray.bind();

			glDrawElements(GL_TRIANGLES, leafIndices.size(), GL_UNSIGNED_INT, 0);
		}
		window.swapBuffers();
		glfwPollEvents();
	}

	window.shutdown();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
