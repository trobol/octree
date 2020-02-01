#include "octree.h"

#define ELEMENTS_PER_NODE 8
#define INDICES_PER_BRANCH 24

#define INDICES_PER_LEAF 6 * 6

#include <math.h>
#include "math/math.h"

const int BRANCH_INDICES[INDICES_PER_BRANCH] = {
	0, 1,
	0, 2,
	1, 3,
	2, 3,

	4, 5,
	4, 6,
	5, 7,
	6, 7,

	0, 4,
	1, 6,
	2, 5,
	3, 7};
const int LEAF_INDICES[INDICES_PER_LEAF] = {
	0, 1, 4,
	6, 1, 4,

	0, 2, 4,
	5, 2, 4,

	2, 3, 5,
	7, 3, 5,

	5, 4, 7,
	6, 4, 7,

	6, 1, 7,
	3, 1, 7,

	1, 0, 3,
	2, 0, 3};
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

//bottom back left = 000
//bottom back right = 001
//bottom front left = 010
//bottom front right = 011
//top	 back  left = 100
//top	 back  right = 101
//top    front left = 110
//top    front right = 111

void Octree::drawNode(Node *node, vec3 v, std::vector<Point> &elements, std::vector<Point> &leafElements)
{
	vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
	vec3 points[ELEMENTS_PER_NODE] = {
		{-1, -1, -1},
		{1, -1, -1},
		{-1, -1, 1},
		{1, -1, 1},

		{-1, 1, -1},
		{-1, 1, 1},
		{1, 1, -1},
		{1, 1, 1}};
	//2 to the power of node size
	float powSize = 1 << node->size;
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
	return setNode(vec3int(x, y, z));
}

Octree::Node *Octree::setNode(vec3int pos)
{
	Node *n = mRootNode;
	vec3int currentPos(0, 0, 0);
	while (n->size > 0)
	{
		bool isRight = pos.x > currentPos.x;
		bool isTop = pos.y > currentPos.y;
		bool isFront = pos.z > currentPos.z;

		int index = (isRight << 2) | (isTop << 1) | isFront;

		//half width of current cube
		int twoPowSizeMinusOne = 1 << (n->size - 1);

		int twoPowHalf = twoPowSizeMinusOne >> 1;
		currentPos += (vec3int(isRight, isTop, isFront) * twoPowSizeMinusOne) - twoPowHalf;

		if (n->subNodes[index] == nullptr)
		{
			n->subNodes[index] = new Node(n->size - 1, n);
			std::cout << "created node size: " << (n->size - 1) << " at " << currentPos << std::endl;
			mNodeCount++;
			if (n->size - 1 == 0)
				mLeafNodeCount++;
		}

		n = n->subNodes[index];
	}
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

	//determine offsets to center
	vec3int offset = vec3int::one * (largetstSize / 2);

	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		setNode(file.mVoxels[i].pos - offset);
		std::cout << file.mVoxels[i].pos - offset << '\n';
	}
	std::cout << std::flush;
}