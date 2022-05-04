#include <octree/graphics/graphics_ctx.h>
#include <unordered_map>

struct ShaderPath {
	std::string frag_path, vert_path;
};

struct GraphicsCtx {
	std::unordered_map<size_t, ShaderPath> m_shader_paths;

	GLuint m_active_shader;

};


Shader GraphicsCtxPtr::GetShader(std::string vert_path, std::string frag_path) {
	return GetShader(vert_path.c_str(), frag_path.c_str());
}

Shader GraphicsCtxPtr::GetShader(const char* vert_path, const char* frag_path) {
	Shader result = Shader::Load(vert_path, frag_path);

	if ((GLuint)result) {
		//ctx->shader_paths.insert({(GLuint)result, { std::string(vert_path), std::string(frag_path)}});
	}

	return result;
}

/*
void GraphicsCtxPtr::ReloadShader(Shader& shader) {

}
*/