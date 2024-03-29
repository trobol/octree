#ifdef WIN32
#define NOMINMAX // why windows
#include <Windows.h>
#endif
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <octree/octree.h>


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <octree/graphics/shader.h>
#include <octree/math/octree_math.h>
#include <octree/math/vec3.h>
#include <octree/math/vec4.h>

#include <math.h>

#include <octree/core/files/filesystem.h>

#include <time.h> /* time */

#include <octree/graphics/buffer.h>
#include <octree/graphics/vertex_attribute_distriptor.h>

#include <octree/graphics/uniform.h>
#include <octree/graphics/vertex_array.h>

#include <octree/drawables/voxel_mesh.h>
#include <octree/drawables/axis_drawable.h>
#include <octree/drawables/mesh_drawable.h>

#include <octree/systems/window.h>
#include <octree/camera.h>

#include <octree/load_obj.h>
#include "defines.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_style.h"

#ifndef ASSET_PATH
#define ASSET_PATH ""
#endif

const std::string ASSET_PATH_STR = ASSET_PATH;

Camera camera;

vec3 center = vec3(0, 0, 0);
bool drawBranches = true;
bool drawLeafs = true;

bool absolute_movement = false;

void draw_ui();

void draw_axis();

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
	if (severity == GL_DEBUG_SEVERITY_HIGH)
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}


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
		camera.mTransform.position = vec3(0, 0, 0);
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
		vbos[(int)i] = vbo;
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
		glDeleteBuffers(1, &vbos[(int)i]);
	}

	return vao;
}

void drawMesh(tinygltf::Model& model, tinygltf::Mesh& mesh) {
	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		glDrawElements(primitive.mode, (GLsizei)indexAccessor.count,
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
	camera.speed = maxf(0.0f, camera.speed + (float)yoffset * 0.05f);
}

Window& window = Window::getInstance();

std::vector<Drawable*> drawables;

struct bin_digit_str {
	char str[8];
};

bin_digit_str bin_digits(uint8_t byte) {
	bin_digit_str result;
	for (uint8_t i = 0; i < 8; i++)
		result.str[i] = ((byte << i) & 0x80) ? '1' : '0';

	return result;
} 

std::vector<Octree*> octrees;
ShaderLoader shaderLoader;
int main(void)
{
	
	srand((unsigned int)time(nullptr));
	VoxFile file;
	MeshDrawable duckDrawable("duck");
	drawables.push_back(&duckDrawable);

	MeshDrawable octreeDrawable("raytrace octree");
	drawables.push_back(&octreeDrawable);
	//octreeDrawable.m_enable = false;
	
	std::vector<Face> faces0;
	std::vector<Face> faces1;
	std::vector<Face> faces2;
	std::vector<Face> faces3;
	load_obj(ASSET_PATH"/models/teapot.obj", faces0);
	load_obj(ASSET_PATH"/models/hand/hand_05.obj", faces1);
	load_obj(ASSET_PATH"/models/hand/hand_10.obj", faces2);
	load_obj(ASSET_PATH"/models/hand/hand_15.obj", faces3);


	std::string filepath = filesystem::fileSelect(ASSET_PATH_STR + "/models/", ".vox");
	file.load(filepath);
	//file.load(ASSET_PATH"/models/small_cube.vox");
	//Octree tree0 = Octree::loadModel(file);

	Octree tree0 = Octree::fromMesh(faces0);
	//Octree tree1 = Octree::fromMesh(faces1);
	//Octree tree2 = Octree::fromMesh(faces2);
	//Octree tree3 = Octree::fromMesh(faces3);

	octrees.push_back(&tree0);
	//octrees.push_back(&tree1);
	//octrees.push_back(&tree2);
	//octrees.push_back(&tree3);
	window.startup();
	// During init, enable debug output
	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );

	tinygltf::Model model;
	filepath = ASSET_PATH_STR + "/models/Duck.gltf";
	if (!loadModel(model, filepath.c_str())) return -1;

	//This should be the model loaded in
	GLuint vao = bindModel(model);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window.mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	setImGuiStyle();

	window.setKeyCallback(key_callback);
	window.setScrollCallback(scroll_callback);
	window.setMouseButtonCallback(mouse_btn_callback);

	const char* vertPath = ASSET_PATH"/shaders/shader.vert";
	const char* fragPath = ASSET_PATH"/shaders/shader.frag";

	const char* vertDuckPath = ASSET_PATH"/shaders/duck.vert";
	const char* fragDuckPath = ASSET_PATH"/shaders/duck.frag";

	const char* vertWireframePath = ASSET_PATH"/shaders/wireframe.vert";
	const char* fragWireframePath = ASSET_PATH"/shaders/wireframe.frag";
	const char* geoWireframePath = ASSET_PATH"/shaders/wireframe.geo";
	glCheckError();
	Shader shaderDuck = shaderLoader.add(vertDuckPath, fragDuckPath);
	shaderDuck.use();
	glCheckError();
	Shader shader = shaderLoader.add(vertPath, fragPath);

	shader.use();
	glCheckError();
	VertexArray testVA = VertexArray::Generate();
	Buffer testVertexBuffer = Buffer::Generate();
	testVA.bind();
	testVertexBuffer.bind(GL_ARRAY_BUFFER);
	uint32_t testPointCount = 0;
	{
		std::vector<vec3> points;
		for ( const Face& face : faces0 ) {
			points.push_back(face.vertices[0].pos);
			points.push_back(face.vertices[1].pos);
			points.push_back(face.vertices[1].pos);
			points.push_back(face.vertices[2].pos);
			points.push_back(face.vertices[2].pos);
			points.push_back(face.vertices[0].pos);
			testPointCount += 6;
		}

		testVertexBuffer.bufferVector(points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);  
	}
	testVA.unbind();
	glCheckError();
	Shader shaderWireframe = shaderLoader.add( vertWireframePath, fragWireframePath);

	UniformMatrix4f wireframeProjMatrix = shaderWireframe.getUniform<mat4>("projMatrix");
	UniformMatrix4f wireframeViewMatrix = shaderWireframe.getUniform<mat4>("viewMatrix");
	Uniform<vec2> wireframeWindowScale = shaderWireframe.getUniform<vec2>("WIN_SCALE");
/*
	puts("");
	for (size_t i = 0; i < tree.m_array.size(); i++) {

		uint32_t node = *(uint32_t*)&tree.m_array[i];
		uint8_t vm = node >> 8;
		uint8_t lm = node;
		printf("%3zu: ptr: %5u valid: %.8s leaf: %.8s full: %08x \n",
			   i, node >> 17,
			   bin_digits(vm).str, 
			   bin_digits(lm).str,
			   node
		);

		fflush(stdout);
	}
	*/

	std::vector<Cube> instances;
	std::vector<Cube> leafInstances;


	tree0.drawNodes(instances, leafInstances);
	
	VoxelMeshDrawable branch_drawable("branch_drawable", true);
	VoxelMeshDrawable leaf_drawable("leaf_drawable", true);

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
	camera.mTransform.position = vec3(1.5, 1.5, 3.0);

	center = vec3(0, 0, 0);

	UniformMatrix4f projMatrix = shader.getUniform<mat4>("projMatrix");
	UniformMatrix4f camMatrix = shader.getUniform<mat4>("camMatrix");

	VertexArray meshVA = VertexArray::Generate();
	Buffer meshVB = Buffer::Generate();

	meshVA.bind();
	meshVB.bind(GL_ARRAY_BUFFER);
	VertexAttributeDiscriptor meshDescriptor;
	meshDescriptor.add(4, GL_FLOAT); // pos
	meshDescriptor.add(4, GL_FLOAT); // norm
	meshDescriptor.add(2, GL_FLOAT); // uv
	meshDescriptor.add(2, GL_FLOAT); // padding
	meshDescriptor.apply();
	{
		vec3 max = faces0[0].vertices[0].pos;
		vec3 min = faces0[0].vertices[0].pos;
		for( Face& f : faces0 ) {
			for (size_t i = 0; i < 3; i++) {
				max = vec3::max( f.vertices[i].pos, max);
				min = vec3::min( f.vertices[i].pos, min);
			}
		}
		for( Face& f : faces0 ) {
		// change the object so it goes from 0->1
			float scale_inv = 1.0f / vec3::largest(max);
			for (size_t i = 0; i < 3; i++) {
				f.vertices[i].pos -= min;
				f.vertices[i].pos *= scale_inv;
				f.vertices[i].pos += vec3( 1, 1, 1);
			}
		}

	}
	meshVB.bufferVector(faces0, GL_STATIC_DRAW);




	meshVA.unbind();
	meshVB.unbind();

	shaderDuck.use();
	UniformMatrix4f duckProjMatrix = shaderDuck.getUniform<mat4>("projMatrix");
	UniformMatrix4f duckViewMatrix = shaderDuck.getUniform<mat4>("viewMatrix");
	ImGuiIO& io = ImGui::GetIO();

	Shader rayShader = shaderLoader.add( ASSET_PATH"/shaders/quad.vert", ASSET_PATH"/shaders/raytrace.frag" );
	rayShader.use();
	puts("## LOADING SHADERS##");
	//shaderLoader.load();
	glCheckError();
	UniformMatrix4f rayProjMatrix = rayShader.getUniform<mat4>("projMatrix");
	UniformMatrix4f rayViewMatrix = rayShader.getUniform<mat4>("viewMatrix");
	Uniform<vec2> rayViewPlanes = rayShader.getUniform<vec2>("viewPlanes");
	Uniform<int> cubeCountU = rayShader.getUniform<int>("cubeCount");
	Uniform<float> otsizeU = rayShader.getUniform<float>("size");

	otsizeU.set((float)tree0.m_size);

	//GLuint tex;
	//glGenTextures(1, &tex);
	//glBindTexture(GL_TEXTURE_1D, tex);
	//glTexImage1D(GL_TEXTURE_1D, 0, GL_R32UI, tree0.m_array.size(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &tree.m_array[0]);
	//glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);



	vec2 viewPlanes = vec2(camera.mNearClip, camera.mFarClip);
	rayViewPlanes.set(viewPlanes);
	
	VertexArray quadVertexArray = VertexArray::Generate();
	Buffer quadVertexBuffer = Buffer::Generate();
	Buffer cubeStorageBuffer = Buffer::Generate();
	quadVertexArray.bind();
	quadVertexBuffer.bind(GL_ARRAY_BUFFER);
	float quadVerts[] = { -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	quadVertexBuffer.bufferArray(quadVerts, 16, GL_STATIC_DRAW);

	VertexAttributeDiscriptor quadDescriptor;
	quadDescriptor.add(2, GL_FLOAT);
	quadDescriptor.add(2, GL_FLOAT);
	quadDescriptor.apply();

	std::vector<vec4> raytraceData;
	raytraceData.reserve(leafInstances.size() * 2);
	for (Cube& cube : leafInstances) {
		vec3 p = cube.pos;
		vec3 c = cube.color;
		raytraceData.push_back(vec4{ p.x, p.y, p.z, 0. });
		raytraceData.push_back(vec4{ c.x, c.y, c.z, 0. });
	}

	cubeStorageBuffer.bind(GL_SHADER_STORAGE_BUFFER);
	cubeCountU = tree0.m_array.size();

	
	//cubeCountU = 1;
	cubeStorageBuffer.bufferVector(tree0.m_array, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cubeStorageBuffer);

	quadVertexArray.unbind();
	quadVertexBuffer.unbind();
	cubeStorageBuffer.unbind();
	//shader.use();
	

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);
	shaderLoader.load();

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	glCheckError();
	size_t i = 0;
	// MAIN LOOP
	while (!window.shouldClose())
	{
		glCheckError();
		i++;
		glEnable(GL_DEPTH_TEST);
		// UPDATE
		camera.bCanCaptureMouse = !io.WantCaptureMouse;
		camera.Update(0);
		

		// DRAW
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (duckDrawable.m_enable) {
			shaderDuck.use();
			duckProjMatrix = camera.getProjMatrix();
			duckViewMatrix = camera.getViewMatrix();
			//drawModel(vao, model);

			meshVA.bind();
			glDrawArrays(GL_TRIANGLES, 0, faces0.size() * 3);
			meshVA.unbind();
		}

//		{
//			testVA.bind();
//			shaderWireframe.use();
//			wireframeProjMatrix = camera.getProjMatrix();
//			wireframeViewMatrix = camera.getViewMatrix();
//			wireframeWindowScale = vec2(0.1f, 0.1f);
//			
//			glDrawArrays(GL_LINES, 0, testPointCount);
//		}
	
		shader.use();
		glCheckError();
		projMatrix = camera.getProjMatrix();
		camMatrix = camera.getViewMatrix();
		for (Drawable* drble : drawables) {
			if (drble->m_enable)
				drble->Draw();
		}



		glDisable(GL_DEPTH_TEST);
		if (octreeDrawable.m_enable) {
			size_t index = (i / 50) % 4;
			Octree* oct = octrees[0];
			//glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
			//glBindTexture(GL_TEXTURE_1D, tex);
			rayShader.use();
			otsizeU.set((float)oct->m_size);
			cubeCountU = oct->m_array.size();
	
			
			rayProjMatrix = camera.getProjMatrix();
			rayViewMatrix = camera.getViewMatrix();
			quadVertexArray.bind();
			cubeStorageBuffer.bind(GL_SHADER_STORAGE_BUFFER);
			cubeStorageBuffer.bufferVector(oct->m_array, GL_STATIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cubeStorageBuffer);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glCheckError();

		draw_ui();
		glCheckError();
		window.swapBuffers();
		glfwPollEvents();
		glCheckError();
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
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Camera");
		float* posp = &camera.mTransform.position.x;
		vec3 rot = camera.mTransform.rotation.ToEuler();
		float* speedp = &camera.speed;
		float* sensp = &camera.sensitivity;
		float* fovp = &camera.mFov;
		ImGui::InputFloat3("Pos:", posp);
		ImGui::InputFloat3("Rot: ", &rot.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::SliderFloat("Speed: ", speedp, 0.01f, 1.0f, "%1.4f");
		ImGui::SliderFloat("Sensitivity: ", sensp, 0.1f, 10.0f, "%4.1f");
		ImGui::SliderFloat("FOV: ", fovp, 10.0f, 110.0f, "%4.0f");
	}

	if (ImGui::CollapsingHeader("Drawables", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (Drawable* drawable : drawables) {
		
			ImGui::Checkbox(drawable->m_name.c_str(), &drawable->m_enable);
		}
	}

	bool reload = false;
	if (ImGui::Button("Reload Shaders"))
	{
		reload = true;
	}

	ImGui::End();
	glCheckError();

	ImGui::Render();
	glCheckError();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glCheckError();

	if (reload) shaderLoader.load();
}
