#include "octree.h"

#define ELEMENTS_PER_NODE 8
#define INDICES_PER_NODE 24
void Octree::drawNodes()
{
	std::vector<vec3> elements(mNodeCount * ELEMENTS_PER_NODE);
	std::vector<int> indices(mNodeCount * INDICES_PER_NODE);
	vec3 v(0, 0, 0);
	Node *node = &mRootNode;
	for (int i = 0; i < mNodeCount; i++)
	{
		elements.push_back();
	}
}

void Octree::drawNode(Node *node, vec3 v, std::vector<vec3> &vector)
{
	if (node == nullptr)
		return;

	int halfSize = node->size / 2;

	drawNode(node->subNodes[0], v + vec3(-1, -1, -1) * halfSize, vector);
	drawNode(node->subNodes[1], v + vec3(1, -1, -1) * halfSize, vector);
	drawNode(node->subNodes[2], v + vec3(-1, -1, 1) * halfSize, vector);
	drawNode(node->subNodes[3], v + vec3(1, -1, 1) * halfSize, vector);

	drawNode(node->subNodes[4], v + vec3(-1, 1, -1) * halfSize, vector);
	drawNode(node->subNodes[5], v + vec3(1, 1, -1) * halfSize, vector);
	drawNode(node->subNodes[6], v + vec3(-1, 1, 1) * halfSize, vector);
	drawNode(node->subNodes[7], v + vec3(1, 1, 1) * halfSize, vector);
}