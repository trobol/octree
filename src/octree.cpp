#include "octree.h"

#define ELEMENTS_PER_NODE 8
#define INDICES_PER_NODE 24

void Octree::drawNodes(std::vector<Point> &elements, std::vector<int> &indices)
{
	elements.reserve(mNodeCount * ELEMENTS_PER_NODE);
	indices.resize(mNodeCount * INDICES_PER_NODE);
	std::cout << elements.size();
	vec3 v(0, 0, 0);
	Node *node = &mRootNode;
	/*
		0 1
		0 3
		1 2
		2 3

		0 4
		1 5
		3 7
		2 6

		4 5
		4 7
		5 6
		6 7
	*/
	const int cubeIndies[INDICES_PER_NODE] = {
		0, 1,
		0, 3,
		1, 2,
		2, 3,

		0, 4,
		1, 5,
		3, 7,
		2, 6,

		4, 5,
		4, 7,
		5, 6,
		6, 7};
	for (int i = 0; i < mNodeCount; i++)
	{
		for (int j = 0; j < INDICES_PER_NODE; j++)
		{
			indices[j + (i * INDICES_PER_NODE)] = cubeIndies[j] + (i * ELEMENTS_PER_NODE);
		}
	}
	drawNode(node, v, elements);
}

void Octree::drawNode(Node *node, vec3 v, std::vector<Point> &elements)
{
	if (node == nullptr)
		return;

	int halfSize = node->size / 2;
	vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
	vec3 points[ELEMENTS_PER_NODE] = {
		{-1, -1, 1},
		{1, -1, 1},
		{1, -1, -1},
		{-1, -1, -1},

		{-1, 1, 1},
		{1, 1, 1},
		{1, 1, -1},
		{-1, 1, -1}};
	for (int i = 0; i < ELEMENTS_PER_NODE; i++)
	{
		Point p;
		p.pos = points[i] * (1 + node->size) + v;
		p.color = color;
		elements.push_back(p);
		std::cout << elements.size() << ' ';
	}
	drawNode(node->subNodes[0], v + vec3(-1, -1, -1) * halfSize, elements);
	drawNode(node->subNodes[1], v + vec3(1, -1, -1) * halfSize, elements);
	drawNode(node->subNodes[2], v + vec3(-1, -1, 1) * halfSize, elements);
	drawNode(node->subNodes[3], v + vec3(1, -1, 1) * halfSize, elements);

	drawNode(node->subNodes[4], v + vec3(-1, 1, -1) * halfSize, elements);
	drawNode(node->subNodes[5], v + vec3(1, 1, -1) * halfSize, elements);
	drawNode(node->subNodes[6], v + vec3(-1, 1, 1) * halfSize, elements);
	drawNode(node->subNodes[7], v + vec3(1, 1, 1) * halfSize, elements);
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
