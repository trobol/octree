#ifndef _OCTREE_H
#define _OCTREE_H

#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"

#include <iostream>
#include <iterator>

#include <octree/core/files/vox_file.h>
#include <octree/load_obj.h>

#pragma pack(push, 1)
struct Cube
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
	float size;
};
#pragma pack(pop)

const size_t s = sizeof(Cube);
struct Point {
	vec3 pos;
	vec3 color;
};

class OctreeChunk;



class OTNode {
private:
	uint32_t data;
public:
	void set_children(uint16_t index);
	void set_valid(uint16_t child_index);
	void set_leaf(uint16_t);
	uint16_t children_ptr; // the lowest bit is the "far" flag
	uint8_t valid_mask;
	uint8_t leaf_mask;
};


struct OTArrayEntry {
	union {
		OTNode node;
		uint32_t fwd_ptr;
	};
};

// this is designed to be used as a pointer
// 
struct OTGroup {
	OTNode nodes[8];
};

class OctreeBuilder;


/*
INFO
For simplicity I am putting bottom a corner at 0 rather than the center
Scale referse to the "level" of the node in the octree, the smallest are scale 0, its parents would be scale 1 etc..
Size referse to the actual world size and is 2^scale of a node
*/
class Octree
{
private:
public:
	uint32_t m_depth; // number of "levels" of nodes
	uint32_t m_size; // 2 ^ m_depth
	
	uint32_t m_alloc_size;


	std::vector<uint32_t> m_array;
	std::vector<uint32_t> m_farpointers; // TEMP HACK

	uint32_t min_depth;

	Octree(uint32_t size);


	static Octree load(std::string path);
	void drawLeaf(uint32_t color, vec3 v, std::vector<Cube>& leafInstances);
	uint32_t setNode(int x, int y, int z, uint32_t value, uint16_t min_depth = 0);
	void fillDepth(uint16_t depth);
	void setNode(vec3int v, vec3 color);

	static Octree fromMesh(std::vector<Face>& face);

	void drawNodes(std::vector<Cube>& instances, std::vector<Cube>& leafInstances);

	void raytrace(vec3 origin, vec3 direction);
	//void drawNode(Node* node, vec3 v, std::vector<Cube>& leafInstances);

	static Octree loadModel(VoxFile& file);
	size_t mLeafNodeCount = 0;
	size_t mNodeCount = 1;
	~Octree()
	{
	}
	
};


#endif