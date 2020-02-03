#include <vector>
#include "math/vec3.h"
#include "math/vec3int.h"

#include <iostream>
#include <iterator>

#include "file/vox_file.h"

struct Point
{
	vec3 pos{0, 0, 0};
	vec3 color{0, 0, 0};
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
	static Octree load(std::string path);
	Node *setNode(int x, int y, int z);
	Node *setNode(vec3 v);
	void drawNodes(std::vector<Point> &elements, std::vector<int> &indices, std::vector<Point> &leafElements, std::vector<int> &leafIndices);

	void drawNode(Node *node, vec3 v, std::vector<Point> &vector, std::vector<Point> &leafElements);

	int getSize()
	{
		return mRootNode->size;
	}
	void loadModel(VoxFile &file);
	Node *mRootNode;
	int mX, mY;
	int mLeafNodeCount = 0;
	int mNodeCount = 1;
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
void printNode(Octree::Node &node, int depth = 0);
