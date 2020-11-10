#include <octree/octree_chunk.h>
#include "defines.h"
#include <iostream>

Node* OctreeChunk::add_node(vec3int pos, vec3 color) {

	uint16_t color_index = add_color(color);
	Node* node = &get_root();
	vec3 currentPos(0, 0, 0); // 0,0,0 is the back bottom left
	for (int size = 5; size > 0; size--)
	{
		float halfThisNode = (float)(1 << size) / 2;

		vec3 centerPos = currentPos + halfThisNode;
		bool greaterX = !(pos.x < centerPos.x);
		bool greaterY = !(pos.y < centerPos.y);
		bool greaterZ = !(pos.z < centerPos.z);

		int local_index = (greaterX << 2) | (greaterY << 1) | greaterZ;

		//half width of current cube

		float halfNextNode = halfThisNode / 2;
		currentPos = currentPos + vec3(greaterX, greaterY, greaterZ) * halfThisNode;

		if (size > 1) {
			if (!node->m_sub_nodes[local_index].m_active)
			{
				uint16_t index = m_nodes.size();
				m_nodes.push_back({});
				node->m_sub_nodes[local_index].m_active = true;
				node->m_sub_nodes[local_index].m_index = index;
				node = &m_nodes.back();
			}
			else {
				uint16_t index = node->m_sub_nodes[local_index].m_index;
				node = &m_nodes[index];
			}
		}
		else {
			if (!node->m_sub_nodes[local_index].m_active)
			{
				m_root_count++;

				node->m_sub_nodes[local_index].m_active = true;
				node->m_sub_nodes[local_index].m_index = color_index;
				if (pos.toFloat() != currentPos)
					std::cout << "goal: " << pos << " result: " << currentPos << '\n';
				return nullptr;
			}
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
		uint32_t sqrt_size = 6 - stack.size();

		unsigned int size = 1 << sqrt_size;
		unsigned int half_size = size / 2;
		if (entry.index > 7) {

			vec3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
			// draw branch
			Cube c;
			c.pos = entry.pos;
			c.color = color;
			c.size = size;
			instances.push_back(c);

			stack.pop_back();
			continue;
		}
		NodeIndex n = entry.node.m_sub_nodes[entry.index];


		vec3 pos = entry.pos + (CUBE_POINTS[entry.index] * half_size);
		if (n.m_active) {
			if (sqrt_size > 1) {
				stack.push_back({ m_nodes[n.m_index], pos });
			}
			else {
				// draw leaf
				Cube c;
				c.pos = pos;
				c.size = 1;
				c.color = m_color_table[n.m_index];
				leafInstances.push_back(c);
				std::cout << "pos: " << pos << " size: " << size << " index: " << entry.index << '\n';
			}

		}
		//std::cout << "pos: " << pos << " size: " << size << " index: " << entry.index << '\n';


		entry.index++;

	}

}