#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"
#include "octree_chunk.h"

#include <iostream>
#include <iterator>

#include <core/files/vox_file.h>

struct Point
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
};
class OctreeChunk;

class Octree
{
private:
	uint32_t m_width;  //multiple of 32
	uint32_t m_height; //multiple of 32
	uint32_t m_chunkCount;
	OctreeChunk *m_chunks;

public:
	Octree(uint32_t width, uint32_t height);

	static Octree load(std::string path);
	void drawLeaf(uint32_t color, vec3 v, std::vector<Point> &leafElements);
	Node *setNode(int x, int y, int z, uint32_t color);
	Node *setNode(vec3 v, uint32_t color);
	void drawNodes(std::vector<Point> &elements, std::vector<int> &indices, std::vector<Point> &leafElements, std::vector<int> &leafIndices);

	void drawNode(Node *node, vec3 v, std::vector<Point> &vector, std::vector<Point> &leafElements);

	void loadModel(VoxFile &file);
	Node *mRootNode;
	size_t mLeafNodeCount = 0;
	size_t mNodeCount = 1;
	~Octree()
	{
		delete mRootNode;
	}

private:
};

class OctreeIterator
{
private:
	int value_;
};
void printNode(Node &node, int depth = 0);
