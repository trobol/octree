#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"

#include <iostream>
#include <iterator>

#include "files/vox_file.h"

struct Point
{
	vec3 pos{ 0, 0, 0 };
	vec3 color{ 0, 0, 0 };
};

class Octree
{
public:
	Octree(int size)
	{
		mRootNode = new Node(size);
	}

	struct Node
	{
		Node(int size) : size{ size } {}
		Node* subNodes[8] = { nullptr };
		unsigned char nodeMask = 0;
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

		int size;
		bool isEmpty()
		{
			for (int i = 0; i < 8; i++)
			{
				if (subNodes[i] == nullptr)
					return false;
			}
			return true;
		}
		~Node()
		{
			if (size == 0) return;
			for (int i = 0; i < 8; i++)
			{
				delete subNodes[i];
			}
		}
	};


	static Octree load(std::string path);
	void Octree::drawLeaf(uint32_t color, vec3 v, std::vector<Point>& leafElements);
	Node* setNode(int x, int y, int z, uint32_t color);
	Node* setNode(vec3 v, uint32_t color);
	void drawNodes(std::vector<Point>& elements, std::vector<int>& indices, std::vector<Point>& leafElements, std::vector<int>& leafIndices);

	void drawNode(Node* node, vec3 v, std::vector<Point>& vector, std::vector<Point>& leafElements);

	int getSize()
	{
		return mRootNode->size;
	}
	void loadModel(VoxFile& file);
	Node* mRootNode;
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
void printNode(Octree::Node& node, int depth = 0);
