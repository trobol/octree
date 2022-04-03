#ifndef _OCTREE_H
#define _OCTREE_H

#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"

#include <iostream>
#include <iterator>

#include <octree/core/files/vox_file.h>



struct Cube
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
	float size;
};

struct Point {
	vec3 pos;
	vec3 color;
};

class OctreeChunk;



struct OTNode {
	uint16_t children_ptr : 15;
	bool children_far : 1;
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
For simplicity I am putting bottom a corner at 0 rather than the center
*/
class Octree
{
private:
	uint32_t m_depth; // number of "levels" of nodes
	uint32_t m_size; // 2 ^ m_depth
	
	uint32_t m_alloc_size;


	std::vector<OTNode> m_array;
	std::vector<uint32_t> m_farpointers; // TEMP HACK

public:
	Octree(uint32_t size);


	static Octree load(std::string path);
	void drawLeaf(uint32_t color, vec3 v, std::vector<Cube>& leafInstances);
	uint32_t setNode(int x, int y, int z, uint16_t min_depth = 0);
	void fillDepth(uint16_t depth);
	void setNode(vec3int v, vec3 color);


	void drawNodes(std::vector<Cube>& instances, std::vector<Cube>& leafInstances);

	//void drawNode(Node* node, vec3 v, std::vector<Cube>& leafInstances);

	static Octree loadModel(VoxFile& file);
	size_t mLeafNodeCount = 0;
	size_t mNodeCount = 1;
	~Octree()
	{
	}
	
};


#endif