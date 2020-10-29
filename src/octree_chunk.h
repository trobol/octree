#ifndef _OCTREE_OCTREE_CHUNK_H
#define _OCTREE_OCTREE_CHUNK_H
#include <cstdint>
#include <vector>

struct NodeIndex
{
	uint16_t index : 31;
	bool flag : 1 = false;
};

struct Node
{
	NodeIndex subNodes[8];
	bool flag;
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
			if (subNodes[i].flag)
				return false;
		}
		return true;
	}
};

/*
32x32 width
*/
class OctreeChunk
{
public:
	Node *get(uint8_t x, uint8_t y);
	Node *add(uint8_t x, uint8_t y, uint8_t z);

	Node *getRootNode()
	{
		return &m_nodes[0];
	}

private:
	std::vector<Node> m_nodes;
};

#endif