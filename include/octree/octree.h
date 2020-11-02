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

class Octree
{
private:
	uint32_t m_width;  //multiple of 32
	uint32_t m_height; //multiple of 32
	uint32_t m_depth;

	uint32_t m_chunk_width;
	uint32_t m_chunk_height;
	uint32_t m_chunk_depth;

	std::vector<OctreeChunk*> m_chunks;

public:
	Octree(uint32_t width, uint32_t height, uint32_t depth);

	static Octree load(std::string path);
	void drawLeaf(uint32_t color, vec3 v, std::vector<Cube>& leafInstances);
	Node* setNode(int x, int y, int z, uint32_t color);
	Node* setNode(vec3int v, uint16_t color);

	OctreeChunk* getChunk(uint32_t x, uint32_t y, uint32_t z);
	void setChunk(OctreeChunk* chunk, uint32_t x, uint32_t y, uint32_t z);

	void drawNodes(std::vector<Cube>& instances, std::vector<Cube>& leafInstances);

	void drawNode(Node* node, vec3 v, std::vector<Cube>& leafInstances);

	static Octree loadModel(VoxFile& file);
	size_t mLeafNodeCount = 0;
	size_t mNodeCount = 1;
	~Octree()
	{
	}

private:
};

class OctreeIterator
{
private:
	int value_;
};
void printNode(Node& node, int depth = 0);
