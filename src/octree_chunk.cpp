#include "octree_chunk.h"



Node* OctreeChunk::add(uint8_t x, uint8_t y, uint8_t z) {

	Node* n = getRootNode();
	vec3 currentPos(0, 0, 0);
	uint8_t size = 32;
	for (int size = mRootNode->size; size != 0; size--)
	{
		float halfThisNode = (1 << n->size) / 2;

		vec3 centerPos = currentPos + halfThisNode;
		bool greaterX = !(pos.x < centerPos.x);
		bool greaterY = !(pos.y < centerPos.y);
		bool greaterZ = !(pos.z < centerPos.z);

		int local_index = (greaterX << 2) | (greaterY << 1) | greaterZ;

		//half width of current cube

		float halfNextNode = halfThisNode / 2;
		currentPos = currentPos + vec3(greaterX, greaterY, greaterZ) * halfThisNode;

		//node is empty, add and set
		if (!n->subNodes[local_index].flag)
		{
			m_nodes.push_back(Node());

			n->subNodes[local_index].flag = true;
			n->subNodes[local_index].index = m_nodes.size() - 1;
			mNodeCount++;
			if (size - 1 == 0)
				mLeafNodeCount++;
			n = m_nodes.end();
		}
		else {
			n = n->subNodes[local_index];
		}


	}
	if (currentPos != pos)
		std::cout << pos << " -> " << currentPos << std::endl;

	return n;


}