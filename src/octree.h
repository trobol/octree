#include <vector>
#include "vec3.h"

#include <iostream>
#include <iterator>

struct Point
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
};

class Octree
{
public:
	Octree(int size) : mRootNode{size} {};
	struct Node
	{
		Node(int size, Node *parent = nullptr) : size{size}, parent{parent} {}
		Node *parent;
		Node *subNodes[8] = {nullptr};
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
			for (int i = 0; i < 8; i++)
			{
				delete subNodes[i];
			}
		}
	};
	Node *setNode(int x, int y, int z)
	{
		Node *n = &mRootNode;
		int currentX = 0, currentY = 0, currentZ = 0;
		while (n->size > 0)
		{
			bool isRight = x > currentX;
			bool isTop = y > currentY;
			bool isFront = z > currentZ;

			int index = (isRight << 2) | (isTop << 1) | isFront;

			int halfSize = n->size / 2;
			currentX += (isRight * 2 - 1) * halfSize;
			currentY += (isTop * 2 - 1) * halfSize;
			currentZ += (isFront * 2 - 1) * halfSize;

			if (n->subNodes[index] == nullptr)
			{
				n->subNodes[index] = new Node(n->size - 1, n);
				std::cout << "created node" << std::endl;
				mNodeCount++;
			}

			n = n->subNodes[index];
		}
		return n;
	}
	void drawNodes();

	void drawNode(Node *node, vec3 v, std::vector<Point> &vector);

	int getSize()
	{
		return mRootNode.size;
	}
	Node mRootNode;
	int mX, mY;
	int mNodeCount = 0;

private:
};

class OctreeIterator
{
private:
	int value_;
};
std::ostream &operator<<(std::ostream &os, const Octree::Node &node);
