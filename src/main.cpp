
#include <Windows.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <octree/octree.h>


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <octree/tiny_gltf.h>

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

bool loadModel(tinygltf::Model& model, const char* filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

std::map<int, GLuint> bindMesh(std::map<int, GLuint> vbos,
	tinygltf::Model& model, tinygltf::Mesh& mesh) {
	for (size_t i = 0; i < model.bufferViews.size(); ++i) {
		const tinygltf::BufferView& bufferView = model.bufferViews[i];
		if (bufferView.target == 0) {  // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;  // Unsupported bufferView.
					   /*
						 From spec2.0 readme:
						 https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
								  ... drawArrays function should be used with a count equal to
						 the count            property of any of the accessors referenced by the
						 attributes            property            (they are all equal for a given
						 primitive).
					   */
		}

		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		std::cout << "bufferview.target " << bufferView.target << std::endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[i] = vbo;
		glBindBuffer(bufferView.target, vbo);

		std::cout << "buffer.data.size = " << buffer.data.size()
			<< ", bufferview.byteOffset = " << bufferView.byteOffset
			<< std::endl;

		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes) {
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride =
				accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			if (vaa > -1) {
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, ((char*)NULL + (accessor.byteOffset)));
			}
			else
				std::cout << "vaa missing: " << attrib.first << std::endl;
		}

		if (model.textures.size() > 0) {
			// fixme: Use material's baseColor
			tinygltf::Texture& tex = model.textures[0];

			if (tex.source > -1) {

				GLuint texid;
				glGenTextures(1, &texid);

				tinygltf::Image& image = model.images[tex.source];

				glBindTexture(GL_TEXTURE_2D, texid);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				GLenum format = GL_RGBA;

				if (image.component == 1) {
					format = GL_RED;
				}
				else if (image.component == 3) {
					format = GL_RGB;
				}
				else {
					// ???
				}

				GLenum type = GL_UNSIGNED_BYTE;
				if (image.bits == 8) {
					// ok
				}
				else if (image.bits == 16) {
					type = GL_UNSIGNED_SHORT;
				}
				else {
					// ???
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
					format, type, &image.image.at(0));
			}
		}
	}

	return vbos;
}

// bind models
void bindModelNodes(std::map<int, GLuint> vbos, tinygltf::Model& model,
	tinygltf::Node& node) {
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		bindMesh(vbos, model, model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		bindModelNodes(vbos, model, model.nodes[node.children[i]]);
	}
}
GLuint bindModel(tinygltf::Model& model) {
	std::map<int, GLuint> vbos;
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
	}

	glBindVertexArray(0);
	// cleanup vbos
	for (size_t i = 0; i < vbos.size(); ++i) {
		glDeleteBuffers(1, &vbos[i]);
	}

	return vao;
}

void drawMesh(tinygltf::Model& model, tinygltf::Mesh& mesh) {
	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		glDrawElements(primitive.mode, indexAccessor.count,
			indexAccessor.componentType,
			((char*)NULL + (indexAccessor.byteOffset)));
	}
}

// recursively draw node and children nodes of model
void drawModelNodes(tinygltf::Model& model, tinygltf::Node& node) {
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		drawMesh(model, model.meshes[node.mesh]);
	}
	for (size_t i = 0; i < node.children.size(); i++) {
		drawModelNodes(model, model.nodes[node.children[i]]);
	}
}
void drawModel(GLuint vao, tinygltf::Model& model) {
	glBindVertexArray(vao);

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		drawModelNodes(model, model.nodes[scene.nodes[i]]);
	}

	glBindVertexArray(0);
}

static void error_callback(int error, const char* description) {
	(void)error;
	fprintf(stderr, "Error: %s\n", description);
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

	tinygltf::Model model;
	if (!loadModel(model, "/models/")) return -1;

	//This should be the model loaded in
	GLuint vao = bindModel(model);

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

