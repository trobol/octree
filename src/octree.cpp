#include <octree/octree.h>



#include <math.h>
#include <octree/math/math.h>
#include <octree/octree_chunk.h>

Octree::Octree(uint32_t width, uint32_t height, uint32_t depth) :
	m_width{ width }, m_height{ height }, m_depth{ depth } {
	m_chunk_width = (width / 32) + 1;
	m_chunk_height = (height / 32) + 1;
	m_chunk_depth = (depth / 32) + 1;

	uint32_t chunk_count = m_chunk_width * m_chunk_height * m_chunk_depth;
	m_chunks.resize(chunk_count);

	for (uint32_t i = 0; i < chunk_count; i++) {
		m_chunks[i] = new OctreeChunk(32);
	}

}

OctreeChunk* Octree::getChunk(uint32_t x, uint32_t y, uint32_t z) {
	uint32_t a = m_chunk_width * m_chunk_height;
	return m_chunks[(z * a) + (y * m_chunk_height) + x];
}

void Octree::setChunk(OctreeChunk* chunk, uint32_t x, uint32_t y, uint32_t z) {
	uint32_t a = m_chunk_width * m_chunk_height;
	m_chunks[(z * a) + (y * m_chunk_height) + x] = chunk;
}

void Octree::drawNodes(std::vector<Cube>& elements, std::vector<Cube>& leafElements)
{
	for (uint32_t x = 0; x < m_chunk_width; x++) {
		for (uint32_t y = 0; y < m_chunk_height; y++) {
			for (uint32_t z = 0; z < m_chunk_depth; z++) {
				OctreeChunk* chunk = getChunk(x, y, z);
				vec3 center = (vec3(x, y, z) * 32) + vec3(16, 16, 16);

				chunk->drawNodes(center, elements, leafElements);
			}
		}
	}
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
	return setNode(vec3int(x, y, z), color);
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


Node* Octree::setNode(vec3int pos, uint16_t color)
{
	// find correct chunk
	vec3int offset = pos / vec3int(32, 32, 32);

	OctreeChunk* chunk = getChunk(offset.x, offset.y, offset.z);

	// calculate position relative to chunk
	vec3int relative_pos = vec3int(32, 32, 32) * offset;

	// call func on chunk
	return (chunk->add_node(relative_pos, color));
}

Octree Octree::loadModel(VoxFile& file)
{

	//get sqrt of nearest power of 2 size
	vec3int size = file.getSize();

	Octree octree(size.x, size.y, size.z);

	union {
		uint16_t color : 15;
		struct {
			uint8_t r : 5;
			uint8_t g : 5;
			uint8_t b : 5;
		} colorRGB;
	};
	union {
		uint32_t color32;
		uint8_t color32split[4];
	};

	uint32_t* palette = file.getPalette();
	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		std::cout << +file.mVoxels[i].colorIndex << '\n';
		color32 = palette[file.mVoxels[i].colorIndex - 1];
		colorRGB.r = color32split[0];
		colorRGB.g = color32split[1];
		colorRGB.b = color32split[2];
		octree.setNode(vec3int(file.mVoxels[i].x, file.mVoxels[i].y, file.mVoxels[i].z), color);
	}
	std::cout << std::flush;

	return octree;
}