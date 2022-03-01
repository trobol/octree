#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"
#include "octree_chunk.h"

#include <iostream>
#include <iterator>

#include <octree/core/files/vox_file.h>

struct Point {
	vec3 pos;
	vec3 color;
};

class OctreeChunk;
struct OctreeNode {
	uint16_t child_offset : 15;
	bool child_far : 1;
	uint8_t valid_mask;
	uint8_t leaf_mask;
};
struct OListEntry {
	union {
		OctreeNode node;
		uint32_t fwd_ptr;
	};
};


/*
For simplicity I am putting bottom a corner at 0 rather than the center
*/
class Octree
{
private:
	uint32_t m_size; // must be power of 2
	uint32_t m_alloc_size;


	OListEntry* m_list;

public:
	Octree(uint32_t size);

	static Octree load(std::string path);
	void drawLeaf(uint32_t color, vec3 v, std::vector<Cube>& leafInstances);
	Node* setNode(int x, int y, int z, uint32_t color);
	Node* setNode(vec3int v, vec3 color);


	void drawNodes(std::vector<Cube>& instances, std::vector<Cube>& leafInstances);

	void drawNode(Node* node, vec3 v, std::vector<Cube>& leafInstances);

	static Octree loadModel(VoxFile& file);
	size_t mLeafNodeCount = 0;
	size_t mNodeCount = 1;
	~Octree()
	{
	}
	
};

class OctreeIterator
{
private:
	int value_;
};
void printNode(Node& node, int depth = 0);
