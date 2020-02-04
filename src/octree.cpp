#include "octree.h"

#define ELEMENTS_PER_NODE 8
#define INDICES_PER_BRANCH 24

#define INDICES_PER_LEAF 36

#include <math.h>
#include "math/math.h"

const int BRANCH_INDICES[INDICES_PER_BRANCH] = {
	0, 1,
	1, 5,
	5, 4,
	4, 0,

	2, 3,
	3, 7,
	7, 6,
	6, 2,

	2, 0,
	3, 1,
	7, 5,
	6, 4};
/*
node layout
x y z
0 0 0
0 0 1
0 1 0
0 1 1
1 0 0
1 0 1
1 1 0
1 1 1

*/
const int LEAF_INDICES[INDICES_PER_LEAF] = {
	0, 1, 2,
	3, 1, 2,

	0, 2, 4,
	6, 2, 4,

	2, 3, 6,
	7, 3, 6,

	5, 4, 7,
	6, 4, 7,

	5, 1, 7,
	3, 1, 7,

	1, 0, 5,
	4, 0, 5};
void Octree::drawNodes(std::vector<Point> &elements, std::vector<int> &indices, std::vector<Point> &leafElements, std::vector<int> &leafIndices)
{

	std::cout << elements.size();
	vec3 v(0, 0, 0);
	Node *node = mRootNode;

	const int branchCount = mNodeCount - mLeafNodeCount;
	const int leafCount = mNodeCount - branchCount;

	elements.reserve(branchCount * ELEMENTS_PER_NODE);
	leafElements.reserve(leafCount * ELEMENTS_PER_NODE);

	indices.resize(branchCount * INDICES_PER_BRANCH);
	leafIndices.resize(leafCount * INDICES_PER_LEAF);

	for (int i = 0; i < branchCount; i++)
	{
		for (int j = 0; j < INDICES_PER_BRANCH; j++)
		{
			indices[j + (i * INDICES_PER_BRANCH)] = BRANCH_INDICES[j] + (i * ELEMENTS_PER_NODE);
		}
	}

	for (int i = 0; i < leafCount; i++)
	{
		for (int j = 0; j < INDICES_PER_LEAF; j++)
		{
			leafIndices[j + (i * INDICES_PER_LEAF)] = LEAF_INDICES[j] + (i * ELEMENTS_PER_NODE);
		}
	}
	drawNode(node, v, elements, leafElements);
}

void Octree::drawNode(Node *node, vec3 v, std::vector<Point> &elements, std::vector<Point> &leafElements)
{
	vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
	vec3 points[ELEMENTS_PER_NODE] = {
		{0, 0, 0},
		{0, 0, 1},
		{0, 1, 0},
		{0, 1, 1},
		{1, 0, 0},
		{1, 0, 1},
		{1, 1, 0},
		{1, 1, 1},
	};
	//2 to the power of node size
	int powSize = 1 << node->size;
	for (int i = 0; i < ELEMENTS_PER_NODE; i++)
	{
		Point p;
		p.pos = points[i] * powSize + v;
		p.color = color;
		if (node->size > 0)
			elements.push_back(p);
		else
			leafElements.push_back(p);
	}
	int halfPowSize = powSize / 2;
	for (int i = 0; i < 8; i++)
	{
		if (node->subNodes[i])
			drawNode(node->subNodes[i], v + points[i] * halfPowSize, elements, leafElements);
	}
}

void printNode(Octree::Node &node, int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << ' ';
	std::cout << '[' << node.size << ']' << std::endl;
	for (int i = 0; i < 8; i++)
	{
		if (node.subNodes[i] != nullptr)
			printNode(*node.subNodes[i], depth + 1);
	}
}

//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
Octree Octree::load(std::string path)
{

	return Octree(1);
}
Octree::Node *Octree::setNode(int x, int y, int z)
{
	return setNode(vec3(x, y, z));
}

/*
node layout
x y z
0 0 0
0 0 1
0 1 0
0 1 1
1 0 0
1 0 1
1 1 0
1 1 1

*/
Octree::Node *Octree::setNode(vec3 pos)
{
	Node *n = mRootNode;
	vec3 currentPos(0, 0, 0);
	while (n->size > 0)
	{
		float halfThisNode = (1 << n->size) / 2;

		vec3 centerPos = currentPos + halfThisNode;
		bool greaterX = !(pos.x < centerPos.x);
		bool greaterY = !(pos.y < centerPos.y);
		bool greaterZ = !(pos.z < centerPos.z);

		int index = (greaterX << 2) | (greaterY << 1) | greaterZ;

		//half width of current cube

		float halfNextNode = halfThisNode / 2;
		currentPos = currentPos + vec3(greaterX, greaterY, greaterZ) * halfThisNode;

		if (n->subNodes[index] == nullptr)
		{
			n->subNodes[index] = new Node(n->size - 1, n);

			mNodeCount++;
			if (n->size - 1 == 0)
				mLeafNodeCount++;
		}

		n = n->subNodes[index];
	}
	if (currentPos != pos)
		std::cout << pos << " -> " << currentPos << std::endl;

	return n;
}

void Octree::loadModel(VoxFile &file)
{
	//get sqrt of nearest power of 2 size
	vec3int size = file.getSize();
	int largetstSize = max(size.x, max(size.y, size.z));
	int sqrtTwoSize = nearestSqrt(largetstSize);

	delete mRootNode;
	mRootNode = new Node(sqrtTwoSize);

	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		setNode(file.mVoxels[i].pos.toFloat());
	}
}