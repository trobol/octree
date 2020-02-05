#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <Windows.h>
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
#include "files/filesystem.h"

#include <time.h> /* time */

const std::string ASSET_PATH_STR = ASSET_PATH;

static void
error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

Camera camera;


vec3 center = vec3(0, 0, 0);
bool drawBranches = true;
bool drawLeafs = true;

float speed = 0.2;

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
		camera.mTransform.rotateAround(center, speed);

	if (key == GLFW_KEY_A)
		camera.mTransform.rotateAround(center, -speed);

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		drawBranches = !drawBranches;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		drawLeafs = !drawLeafs;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	speed += yoffset * 0.05;
}
Shader shader;

void drawFile(std::vector<Point>& elements, std::vector<int>& indices, VoxFile& file);
void window_size_callback(GLFWwindow* window, int width, int height)
{
	camera.mAspectRatio = (float)width / height;
}

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

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Octree render", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gl_lite_init();
	glfwSwapInterval(1);

	// NOTE: OpenGL error checks have been omitted for brevity


	shader = Shader::Load(ASSET_PATH_STR + "/shaders/shader.vert", ASSET_PATH_STR + "/shaders/shader.frag");

	glUseProgram(shader);

	std::vector<Point> elements;
	std::vector<int> indices;

	std::vector<int> leafIndices;
	std::vector<Point> leafElements;

	tree.drawNodes(elements, indices, leafElements, leafIndices);
	//drawFile(leafElements, leafIndices, file);
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glUseProgram(shader);

	camera.mTransform.scale = vec3(1, 1, 1);
	camera.mTransform.position = vec3(0, 0, 10.);

	center = file.getSize().toFloat() / 2;
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

		glUniformMatrix4fv(projMatrix, 1, GL_FALSE, camera.getProjMatrix());

		glUniformMatrix4fv(camMatrix, 1, GL_FALSE, mat4::lookAt(camera.mTransform.position, center, vec3::up));

		//draw branches

		if (drawBranches)
		{
			glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(Point), elements.data(), GL_DYNAMIC_DRAW);

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);

			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		if (drawLeafs)
		{
			glBufferData(GL_ARRAY_BUFFER, leafElements.size() * sizeof(Point), leafElements.data(), GL_DYNAMIC_DRAW);

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, leafIndices.size() * sizeof(int), leafIndices.data(), GL_DYNAMIC_DRAW);

			glDrawElements(GL_TRIANGLES, leafIndices.size(), GL_UNSIGNED_INT, 0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

vec3 points[8] = {
	{-1, -1, -1},
	{1, -1, -1},
	{-1, -1, 1},
	{1, -1, 1},

	{-1, 1, -1},
	{-1, 1, 1},
	{1, 1, -1},
	{1, 1, 1} };
const int LEAF_INDICES[36] = {
	0, 1, 4,
	6, 1, 4,

	0, 2, 4,
	5, 2, 4,

	2, 3, 5,
	7, 3, 5,

	5, 4, 7,
	6, 4, 7,

	6, 1, 7,
	3, 1, 7,

	1, 0, 3,
	2, 0, 3 };
void drawFile(std::vector<Point>& elements, std::vector<int>& indices, VoxFile& file)
{
	elements.reserve(file.getNumVoxels() * 8);
	indices.resize(file.getNumVoxels() * 36);
	for (int i = 0; i < file.getNumVoxels(); i++)
	{

		for (int j = 0; j < 36; j++)
		{
			indices[j + (i * 36)] = LEAF_INDICES[j] + (i * 8);
		}
		vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
		vec3 pos = vec3(file.mVoxels[i].x, file.mVoxels[i].y, file.mVoxels[i].z);
		for (int k = 0; k < 8; k++)
		{
			Point p;
			p.pos = pos + (points[k] * 0.5f);
			p.color = color;

			elements.push_back(p);
		}
		std::cout << pos << '\n';
	}
}
