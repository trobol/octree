#ifndef _OCTREE_OCTREE_CHUNK_H
#define _OCTREE_OCTREE_CHUNK_H
#include <cstdint>
#include <vector>

#include <octree/math/vec3.h>
#include <octree/math/vec3int.h>
/*
	a 15 bit index allows for up to 32,768,
	the same number of leaf nodes in a 32 octree
	the leaves are stored separately 
*/
struct NodeIndex
{
	bool m_active : 1 ;
	union {
		uint16_t m_index : 15;
	};
	NodeIndex() : m_active{false}, m_index{0} {}
};

struct Cube
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
	float size;
};

struct Node
{
	NodeIndex m_sub_nodes[8];
	/*
		top down
		top 4 5    bottom 0 1
			6 7			  2 3
		front


		//bottom back left = 000
		//bottom back right = 001
		//bottom front left = 010
		//bottom front right = 011
		//top	 back  left = 100
		//top	 back  right = 101
		//top    front left = 110
		//top    front right = 111

	*/

	bool isEmpty()
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_sub_nodes[i].m_active)
				return false;
		}
		return true;
	}
};

class LeafNode {};
/*
32x32 width
*/
class OctreeChunk
{
private:
	std::vector<Node> m_nodes;
	//std::vector<LeafNode> m_leaves;
	unsigned int m_root_size;

	unsigned int m_root_count;

	std::vector<vec3> m_color_table;
public:
	OctreeChunk(unsigned int root_size) : m_root_count{0}, m_root_size{root_size} {m_nodes.push_back({});}
	OctreeChunk(OctreeChunk&&) = default;
	Node &get_node(uint8_t x, uint8_t y);
	Node *add_node(uint8_t x, uint8_t y, uint8_t z);
	
	Node* add_node(vec3int pos, vec3 color);

	Node* get_root()
	{
		return &m_nodes[0];
	}

	size_t add_color(vec3 color) {
		size_t index = m_color_table.size();
		m_color_table.push_back(color);
		return index;
	}
	void drawNodes(vec3 center, std::vector<Cube> &instances, std::vector<Cube>& leafInstances);
};

#endif