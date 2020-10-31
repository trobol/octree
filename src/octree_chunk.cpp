#include <octree/octree_chunk.h>
#include "defines.h"
#include <iostream>

Node* OctreeChunk::add_node(vec3int pos, uint16_t color) {

	Node* node = &get_root();
	vec3 currentPos(0, 0, 0); // 0,0,0 is the back bottom left
	for (int size = m_root_size; size > 0; size--)
	{
		float halfThisNode = (1 << size) / 2;

		vec3 centerPos = currentPos + halfThisNode;
		bool greaterX = !(pos.x < centerPos.x);
		bool greaterY = !(pos.y < centerPos.y);
		bool greaterZ = !(pos.z < centerPos.z);

		int local_index = (greaterX << 2) | (greaterY << 1) | greaterZ;

		//half width of current cube

		float halfNextNode = halfThisNode / 2;
		currentPos = currentPos + vec3(greaterX, greaterY, greaterZ) * halfThisNode;

		//node is empty, add and set
		if (!node->m_sub_nodes[local_index].m_active)
		{
			uint16_t index;
			if (size > 0) {
				index = m_nodes.size();
				m_nodes.push_back({});

			}
			else {
				index = color;
			}

			node->m_sub_nodes[local_index].m_active = true;
			node->m_sub_nodes[local_index].m_index = index;
			node = &m_nodes.back();
		}
		else {
			uint16_t index = node->m_sub_nodes[local_index].m_index;
			node = &m_nodes[index];
		}


	}


	return node;
}

void OctreeChunk::drawNodes(vec3 center, std::vector<Cube>& instances, std::vector<Cube>& leafInstances)
{
	struct NodeStackEntry {
		Node& node;
		vec3 pos;
		uint32_t index = 0;
	};
	std::vector<NodeStackEntry> stack;
	stack.reserve(5);
	stack.push_back({ get_root(), center });

	while (!stack.empty()) {

		NodeStackEntry& entry = stack.back();

		if (entry.index > 7) {

			vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
			// draw branch
			Cube c;
			c.pos = entry.pos;
			c.color = color;
			leafInstances.push_back(c);

			stack.pop_back();
		}
		NodeIndex n = entry.node.m_sub_nodes[entry.index];
		uint32_t size = 5 - stack.size();
		uint32_t half_2_pow_size = 1 << (size - 1);

		vec3 pos = entry.pos + (CUBE_POINTS[n.m_index] * half_2_pow_size);
		if (n.m_active) {
			if (size > 0) {
				stack.push_back({ m_nodes[n.m_index], pos });
			}
			else {
				// draw leaf
				Cube c;
				c.pos = pos;
				c.color = vec3(n.m_color.r, n.m_color.g, n.m_color.b);
				leafInstances.push_back(c);
			}
		}

		std::cout << "pos: " << pos << " size: " << size << " index: " << entry.index << '\n';

		entry.index++;
		
	}

}