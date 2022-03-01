#include <octree/octree.h>



#include <math.h>
#include <octree/math/math.h>
#include <octree/octree_chunk.h>

Octree::Octree(uint32_t size) {
	m_size = size;
	m_alloc_size = 0;
	for (uint32_t s = size; s > 0; s = s >> 1)
		m_alloc_size += s * s * s;
	m_list = new OListEntry[m_alloc_size];
}




void Octree::drawNodes(std::vector<Cube>& elements, std::vector<Cube>& leafElements)
{
	/*
	for (uint32_t x = 0; x < m_chunk_width; x++) {
		for (uint32_t y = 0; y < m_chunk_height; y++) {
			for (uint32_t z = 0; z < m_chunk_depth; z++) {
				OctreeChunk* chunk = getChunk(x, y, z);
				vec3 center = (vec3(x, y, z) * 32) + vec3(16, 16, 16);

				chunk->drawNodes(center, elements, leafElements);
			}
		}
	}
	*/
}


/*
void printNode(Node &node, int depth)
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
*/

/*
//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
Octree Octree::load(std::string path)
{
	return Octree(1);
}
*/

Node* Octree::setNode(int x, int y, int z, uint32_t color)
{
	return setNode(vec3int(x, y, z), vec3(0, 0, 0));
}

/*
node layout
x y z
0 0 0
0 0 1
0 1 0
0 1 1
1 0 0
1 0 1
1 1 0
1 1 1

*/


Node* Octree::setNode(vec3int pos, vec3 color)
{
	// find correct chunk
	vec3int offset = pos / vec3int(32, 32, 32);

	//OctreeChunk* chunk = getChunk(offset.x, offset.y, offset.z);

	// calculate position relative to chunk
	vec3int relative_pos = vec3int(32, 32, 32) * offset;

	// call func on chunk
	//return (chunk->add_node(pos, color));
	return NULL;
}

Octree Octree::loadModel(VoxFile& file)
{

	//get sqrt of nearest power of 2 size
	vec3int size = file.getSize();
	int maxSize = std::max(size.x, std::max(size.y, size.z));

	Octree octree(maxSize);

	union {
		uint32_t color32;
		uint8_t color32split[4];
	};

	uint32_t* palette = file.getPalette();
	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		color32 = palette[file.mVoxels[i].colorIndex - 1];
		vec3 color = vec3(color32split[0], color32split[1], color32split[2]) / 255;

		//dont forget to convert to correct space
		octree.setNode(vec3int(file.mVoxels[i].x, file.mVoxels[i].y, file.mVoxels[i].z), color);
	}

	return octree;
}