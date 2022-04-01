#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <octree/octree.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <octree/graphics/shader.h>
#include <octree/math/octree_math.h>
#include <octree/math/vec3.h>

#include <math.h>

#include <octree/core/files/filesystem.h>

#include <time.h> /* time */

#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>

#include <octree/graphics/uniform.h>
#include <octree/graphics/vertex_array.h>

#include <octree/drawables/voxel_mesh.h>
#include <octree/drawables/axis_drawable.h>

#include <octree/systems/window.h>
#include <octree/camera.h>
#include "defines.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_style.h"

const std::string ASSET_PATH_STR = ASSET_PATH;

Camera camera;

vec3 center = vec3(0, 0, 0);
bool drawBranches = true;
bool drawLeafs = true;

bool absolute_movement = false;

void draw_ui();

void draw_axis();


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	// VISUALIZATION
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		drawBranches = !drawBranches;
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		drawLeafs = !drawLeafs;




	if (key == GLFW_KEY_EQUAL)
		camera.mFov -= 2;
	if (key == GLFW_KEY_MINUS)
		camera.mFov += 2;


	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.mTransform.position = vec3(0, 0, 10);
		camera.mTransform.rotation = Quaternion::identity;
	}
}

static void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods) {
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, 0, 0);
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.speed = maxf(0.0, camera.speed + yoffset * 0.05);
}

Shader shader;

Window& window = Window::getInstance();

std::vector<Drawable*> drawables;

int main(void)
{
	srand(time(NULL));
	VoxFile file;

	std::string filepath = filesystem::fileSelect(ASSET_PATH_STR + "/models/", ".vox");
	file.load(filepath);
	//file.load("../../assets/box.vox");
	Octree tree = Octree::loadModel(file);

	window.startup();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window.mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	setImGuiStyle();

	window.setKeyCallback(key_callback);
	window.setScrollCallback(scroll_callback);
	window.setMouseButtonCallback(mouse_btn_callback);

	std::string vertPath = ASSET_PATH_STR + "/shaders/shader.vert";
	std::string fragPath = ASSET_PATH_STR + "/shaders/shader.frag";
	shader = Shader::Load(vertPath, fragPath);

	shader.use();


	std::vector<Cube> instances;
	std::vector<Cube> leafInstances;


	tree.drawNodes(instances, leafInstances);

	VoxelMeshDrawable branch_drawable("branch_drawable", true);
	VoxelMeshDrawable leaf_drawable("leaf_drawable", false);

	branch_drawable.Initialize();
	branch_drawable.SetInstances(instances);

	leaf_drawable.Initialize();
	leaf_drawable.SetInstances(leafInstances);

	drawables.push_back(&branch_drawable);
	drawables.push_back(&leaf_drawable);
	
	/*
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
	*/

	AxisDrawable axis_drawable;
	axis_drawable.Initialize();
	drawables.push_back(&axis_drawable);


	camera.mTransform.scale = vec3(1, 1, 1);
	camera.mTransform.position = vec3(-10.0, 0, -10.);

	center = vec3(0, 0, 0);

	UniformMatrix4f projMatrix(shader, "projMatrix");
	UniformMatrix4f camMatrix(shader, "camMatrix");


	ImGuiIO& io = ImGui::GetIO();

	glEnable(GL_DEPTH_TEST);

	// MAIN LOOP
	while (!window.shouldClose())
	{


		// UPDATE
		camera.bCanCaptureMouse = !io.WantCaptureMouse;
		camera.Update(0);


		// DRAW
		shader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		projMatrix = camera.getProjMatrix();
		camMatrix = camera.getViewMatrix();

		for (Drawable* drble : drawables) {
			if (drble->m_enable)
				drble->Draw();
		}

		

		draw_ui();

		window.swapBuffers();
		glfwPollEvents();
	}

	
	window.shutdown();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}



void startup() {

}


void shutdown() {

}

void draw_ui() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	//ImGui::ShowDemoWindow();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Camera");
		float* posp = &camera.mTransform.position.x;
		vec3 rot = camera.mTransform.rotation.ToEuler();
		float* speedp = &camera.speed;
		float* sensp = &camera.sensitivity;
		float* fovp = &camera.mFov;
		ImGui::InputFloat3("Pos:", posp);
		ImGui::InputFloat3("Rot: ", &rot.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::SliderFloat("Speed: ", speedp, 0.1f, 10.0f, "%4.1f");
		ImGui::SliderFloat("Sensitivity: ", sensp, 0.1f, 10.0f, "%4.1f");
		ImGui::SliderFloat("FOV: ", fovp, 10.0f, 110.0f, "%4.0f");
	}

	if (ImGui::CollapsingHeader("Drawables")) {
		for (Drawable* drawable : drawables) {
			if (!ImGui::TreeNode(drawable->m_name.c_str())) continue;
			
			ImGui::Checkbox("enabled:", &drawable->m_enable);

			ImGui::TreePop();
		}
	}

	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
