#ifndef _OCTREE_LOADOBJ_H
#define _OCTREE_LOADOBJ_H

#include <vector>
#include <octree/math/vec3.h>
#include <octree/math/vec2.h>

struct Vertex {
	vec3 pos;
	vec3 norm;
	vec2 uv;
};
struct Face {
	Vertex vertices[3];
};

bool load_obj(const char* path, std::vector<Face>& faces);


#endif