
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

void draw_axis();

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

	if (key == GLFW_KEY_EQUAL)
		camera.mFov -= 2;
	if (key == GLFW_KEY_MINUS)
		camera.mFov += 2;


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

	Buffer branchInstanceBuffer = Buffer::Generate();
	branchInstanceBuffer.bind(GL_ARRAY_BUFFER);
	branchInstanceBuffer.bufferVector(instances, GL_STATIC_DRAW);
	branchInstanceBuffer.unbind();


	VertexArray branchVertexArray = VertexArray::Generate();
	Buffer branchVertexBuffer = Buffer::Generate();
	Buffer branchElementBuffer = Buffer::Generate();


	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	branchVertexArray.bind();
	branchVertexBuffer.bind(GL_ARRAY_BUFFER);
	branchElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

	branchVertexBuffer.bufferArray(CUBE_POINTS, ELEMENTS_PER_CUBE, GL_STATIC_DRAW);
	branchElementBuffer.bufferArray(BRANCH_INDICES, INDICES_PER_BRANCH, GL_STATIC_DRAW);



	VertexAttributeDiscriptor discriptor;
	discriptor.add(3, GL_FLOAT); // position attribute
	//discriptor.add(3, GL_FLOAT); moved to instance
	discriptor.apply();

	//instanced attributes
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	branchInstanceBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
	branchInstanceBuffer.unbind();
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

	branchVertexArray.unbind();
	branchVertexBuffer.unbind();
	branchElementBuffer.unbind();



	// LEAF BUFFERS

	Buffer leafInstanceBuffer = Buffer::Generate();
	leafInstanceBuffer.bind(GL_ARRAY_BUFFER);
	leafInstanceBuffer.bufferVector(leafInstances, GL_STATIC_DRAW);
	leafInstanceBuffer.unbind();

	VertexArray leafVertexArray = VertexArray::Generate();
	Buffer leafVertexBuffer = Buffer::Generate();
	Buffer leafElementBuffer = Buffer::Generate();

	leafVertexArray.bind();
	leafVertexBuffer.bind(GL_ARRAY_BUFFER);
	leafElementBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);



	leafVertexBuffer.bufferArray(CUBE_POINTS, ELEMENTS_PER_CUBE, GL_STATIC_DRAW);
	leafElementBuffer.bufferArray(LEAF_INDICES, INDICES_PER_LEAF, GL_STATIC_DRAW);


	discriptor.apply();


	//instanced attributes
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	leafInstanceBuffer.bind(GL_ARRAY_BUFFER);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
	leafInstanceBuffer.unbind();
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.


	leafVertexArray.unbind();
	leafVertexBuffer.unbind();
	leafElementBuffer.unbind();

	/*
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
	*/

	VertexArray axisVertexArray = VertexArray::Generate();
	Buffer axisVertexBuffer = Buffer::Generate();

	axisVertexArray.bind();
	axisVertexBuffer.bind(GL_ARRAY_BUFFER);

	VertexAttributeDiscriptor axisDiscriptor;
	axisDiscriptor.add(3, GL_FLOAT); // vertex position attribute
	axisDiscriptor.add(3, GL_FLOAT); // instance position attribute
	axisDiscriptor.add(3, GL_FLOAT); // color attribute
	axisDiscriptor.add(1, GL_FLOAT); // size
	axisDiscriptor.apply();

	float axisVertexData[60] = {
		// x, y, z  r, g, b
		0, 0, 0,	0, 0, 0,	1, 0, 0,	5,
		1, 0, 0,	0, 0, 0,	1, 0, 0,	5,
		0, 0, 0,	0, 0, 0,	0, 1, 0,	5,
		0, 1, 0,	0, 0, 0,	0, 1, 0,	5,
		0, 0, 0,	0, 0, 0,	0, 0, 1,	5,
		0, 0, 1,	0, 0, 0,	0, 0, 1,	5,
	};
	
	axisVertexBuffer.bufferArray(axisVertexData, 60, GL_STATIC_DRAW);
	axisVertexArray.unbind();
	axisVertexBuffer.unbind();

	camera.mTransform.scale = vec3(1, 1, 1);
	camera.mTransform.position = vec3(0, 0, 10.);

	center = vec3(0, 0, 0);

	UniformMatrix4f projMatrix(shader, "projMatrix");
	UniformMatrix4f camMatrix(shader, "camMatrix");

	glEnable(GL_DEPTH_TEST);
	while (!window.shouldClose())
	{
		shader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projMatrix = camera.getProjMatrix();
		camMatrix = mat4::lookAt(camera.mTransform.position, center, vec3::up);

		//draw branches

		if (drawBranches)
		{

			branchVertexArray.bind();
			glLineWidth(1);
			glDrawElementsInstanced(GL_LINES, INDICES_PER_BRANCH, GL_UNSIGNED_INT, 0, instances.size());
		}
		if (drawLeafs)
		{
			leafVertexArray.bind();

			glDrawElementsInstanced(GL_TRIANGLES, INDICES_PER_LEAF, GL_UNSIGNED_INT, 0, leafInstances.size());
		}

		//draw axis
		glLineWidth(10);
		axisVertexArray.bind();
		glDrawArrays(GL_LINES, 0, 6);

		window.swapBuffers();
		glfwPollEvents();
	}

	window.shutdown();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

