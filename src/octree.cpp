#include <octree/octree.h>
#include <stack>


#include <math.h>
#include <octree/math/octree_math.h>

#include <octree/octree_builder.h>

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
unsigned int next_pow2(unsigned int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

Octree::Octree(uint32_t size) :
m_depth{0},
m_size{0}
{

	// calculate the depth that can hold all the data
	while(((uint32_t)1 << m_depth) < size) { m_depth++; }
	m_size = 1 << m_depth;

	
	m_alloc_size = 0;
	for (uint32_t s = size; s > 0; s = s >> 1)
		m_alloc_size += s * s * s;

	m_array.resize(1);

	m_array[0].children_ptr = 0;
	m_array[0].children_far = 0;
	m_array[0].valid_mask = 0;
	m_array[0].leaf_mask = 0;

	printf("size %u\n", m_size);
}

// out_str should be at least 9 bytes
void bitArrayToStr(uint8_t bits, char* out_str) {
	out_str[0] = '0' + ((bits & 0x80) >> 7);
	out_str[1] = '0' + ((bits & 0x40) >> 6);
	out_str[2] = '0' + ((bits & 0x20) >> 5);
	out_str[3] = '0' + ((bits & 0x10) >> 4);
	out_str[4] = '0' + ((bits & 0x08) >> 3);
	out_str[5] = '0' + ((bits & 0x04) >> 2);
	out_str[6] = '0' + ((bits & 0x02) >> 1);
	out_str[7] = '0' + ((bits & 0x01) >> 0);
	out_str[8] = 0;
}

// elements are the box outlines
// leafElements are the cubes that makes the actual model
void Octree::drawNodes(std::vector<Cube>& elements, std::vector<Cube>& leafElements)
{

	printf("rendering %llu nodes\n", m_array.size());
	vec3 random_colors[256] = {};
	for (uint16_t i = 0; i < 256; i++) 
		random_colors[i] = vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
	
	struct NodeStackEntry {
		const uint32_t index;
		const int pos_x, pos_y, pos_z;
		uint8_t child_offset;
		uint32_t depth;
	};
	std::stack<NodeStackEntry> traversal_stack;
	traversal_stack.push({ 0, 0, 0, 0, 0, m_depth });

	//uint32_t current_size = m_size;

	while (!traversal_stack.empty()) {

		uint8_t child_offset;
		uint32_t index;
		int x;
		int y;
		int z;
		uint32_t current_depth;
		{
			NodeStackEntry& entry = traversal_stack.top();
			child_offset = entry.child_offset;
			index = entry.index;
			x = entry.pos_x;
			y = entry.pos_y;
			z = entry.pos_z;
			entry.child_offset++;
			current_depth = entry.depth;
		}

		// done with this node (and all its children)
		if (child_offset > 7) {

			// draw branch
			Cube c;
			c.pos = vec3((float)x, (float)y, (float)z);
			c.color = random_colors[index % 256];
			c.size = (float)(1 << current_depth);
			traversal_stack.pop();

			elements.push_back(c);
			continue;
		}
		
		OTNode parent = m_array[index];
		int half_size = 1 << (current_depth-1);

		uint32_t children_ptr = parent.children_ptr;
		//if (parent.children_far) children_ptr = m_farpointers[children_ptr];
		uint32_t child_index = index + children_ptr + child_offset;
		

		int pos_x[8] = {x + half_size, x, x + half_size, x, x + half_size, x, x + half_size, x};
		int pos_y[8] = {y + half_size, y + half_size, y, y, y + half_size, y + half_size, y, y};
		int pos_z[8] = {z + half_size, z + half_size, z + half_size, z + half_size, z, z, z, z};

		if (!(parent.valid_mask & (1 << child_offset))) continue;
		if (parent.leaf_mask & (1 << child_offset)) {
			Cube c;
			c.pos = vec3((float)pos_x[child_offset], (float)pos_y[child_offset], (float)pos_z[child_offset]);
			c.size = (float)half_size;
			c.color = random_colors[child_index % 256];
			leafElements.push_back(c);
			continue;
		} 

		{
			// if non-leaf, add to elements and push to traversal_stack
			uint8_t of = child_offset;	
			traversal_stack.push({child_index, pos_x[of], pos_y[of], pos_z[of], 0, current_depth-1});
		}	
	}

	printf("drew %llu branches and %llu leaves", elements.size(), leafElements.size());
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

// we should make a custom dynamic array/buffer for storing the octree
// along with a "view" class to make the code a bit cleaner

// allocate the memory for nodes up to a depth, so that we dont overflow our pointers as soon

uint32_t Octree::setNode(int target_x, int target_y, int target_z, uint16_t min_depth)
{
	// ensure x, y, z is inside octree

	
	

	// child offset is basically 3 bits representing if x, y or z are larger than center, 00000zyx
	// it can be interpreted as an int and the following arrays allow for translation into coords
	// [x + half_size, x, x + half_size, x, x + half_size, x, x + half_size, x]
	// [y + half_size, y + half_size, y, y, y + half_size, y + half_size, y, y]
	// [z + half_size, z + half_size, z + half_size, z + half_size, z, z, z, z]
	

	
	uint32_t current_index = 0;
	uint32_t parent_index = 0;
	uint8_t child_offset = 0;

	vec3 currentPos(0, 0, 0); // 0,0,0 is the back bottom left
	int x = 0;
	int y = 0;
	int z = 0;

	uint32_t current_size = m_size;
	while(current_size > 1)
	{
	
		parent_index = current_index;

		// get half size
		int half_size = current_size >> 1;

		int center_x = x + half_size;
		int center_y = y + half_size;
		int center_z = z + half_size;

		// child positions
		int pos_x[8] = {x + half_size, x, x + half_size, x, x + half_size, x, x + half_size, x};
		int pos_y[8] = {y + half_size, y + half_size, y, y, y + half_size, y + half_size, y, y};
		int pos_z[8] = {z + half_size, z + half_size, z + half_size, z + half_size, z, z, z, z};

		

		// see child offset comment above
		child_offset = ((target_z < center_z) << 2) | ((target_y < center_y) << 1) | (target_x < center_x);
		
		
		// no valid children
		// we are going to make some children valid, so allocate them
		if (!m_array[current_index].valid_mask && half_size > 1) {
			uint32_t ptr = (uint32_t)m_array.size() - current_index;
			if (ptr > (1 << 14)) puts("ERROR: node index will overflow");
		
			m_array[current_index].children_ptr = ptr;
			m_array.resize(m_array.size() + 8); 
		}
		// assume that parent node is already a valid node
		// this means all members are valid values and has space designated for children, even if it has no valid children
		bool child_valid;
		{ // prevents parent reference usage after array resize
			OTNode& parent = m_array[parent_index];
			// TODO: mask check should be a function
			// is the child we want to work on next valid?
			child_valid = parent.valid_mask & (1 << child_offset);
			// we are gonna make the child valid if its not
			parent.valid_mask |= 1 << child_offset;

			uint32_t children_ptr = parent.children_ptr;
			if (parent.children_far) children_ptr = m_farpointers[children_ptr];
			current_index = parent_index + children_ptr + child_offset;
		}


		x = pos_x[child_offset];
		y = pos_y[child_offset];
		z = pos_z[child_offset];
		
		//printf("halfsize: %5i\n", half_size);

		current_size = half_size;
	}

	
	if (min_depth == 0)
		m_array[parent_index].leaf_mask |= 1 << child_offset;
	if (target_x != x || target_y != y || target_z != z) puts("ERROR: target did not match destination in octree");

	return current_index;
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


void Octree::setNode(vec3int pos, vec3 color)
{
	// find correct chunk
	vec3int offset = pos / vec3int(32, 32, 32);

	//OctreeChunk* chunk = getChunk(offset.x, offset.y, offset.z);

	// calculate position relative to chunk
	vec3int relative_pos = vec3int(32, 32, 32) * offset;

	// call func on chunk
	//return (chunk->add_node(pos, color));
	setNode(pos.x, pos.y, pos.z);
	
}





Octree Octree::loadModel(VoxFile& file)
{

	//get sqrt of nearest power of 2 size
	vec3int size = file.getSize();
	int maxSize = std::max(size.x, std::max(size.y, size.z));
	unsigned int realSize = next_pow2(maxSize);
	Octree octree(maxSize);

	union {
		uint32_t color32;
		uint8_t color32split[4];
	};

	/*
	if (realSize > 32) {
		int min_depth = 5;
		int step = 1 << (min_depth-1); // 2 pow min_depth
		for (int x = 0; x < realSize; x+=step) {
			for (int y = 0; y < realSize; y+=step) {
				for (int z = 0; z < realSize; z+=step) {
					octree.setNode(x, y, z, min_depth);
				}
			}
		}
	}
	*/

	OctreeBuilder builder((uint32_t)std::sqrt(realSize));
	uint32_t* palette = file.getPalette();
	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		color32 = palette[file.mVoxels[i].colorIndex - 1];
		vec3 color = vec3(color32split[0], color32split[1], color32split[2]) / 255;

		//if (file.mVoxels[i].x > 50 || file.mVoxels[i].y > 50 || file.mVoxels[i].z > 50) continue;
		//dont forget to convert to correct space
		octree.setNode(file.mVoxels[i].x, file.mVoxels[i].y, file.mVoxels[i].z);
	}

	printf("\nCOUNT: %u\n", (unsigned int)octree.m_array.size());
	/*
	for(int i = 0; i <  builder.nodeLevels.size(); i++ ) {
		std::vector<OTNode>& level = builder.nodeLevels[i];
		printf("[%i]: ");
		for(int j = 0; j < level.size(); j++) {
			OTNode node = level[j];
			printf("%i, ", node.children_ptr);
		}
		printf("\n");
	}

	puts("\n");

	for (int i = builder.nodeLevels.size()-1; i >= 0; i--) {
		std::vector<OTNode>& level = builder.nodeLevels[i];

		for(int j = 0; j < level.size(); j++) {
			OTNode node = level[j];
			node.children_ptr += level.size() - j;
			octree.m_array.push_back(node);
			printf("%i, ", node.children_ptr);
		}
	}
	*/

	/*
	for (unsigned int i = 0; i < octree.m_array.size(); i++) {
		printf("[%u]: {\n", i);
		OTNode node = octree.m_array[i];
		char tmp[9];
		bitArrayToStr(node.valid_mask, tmp);
		printf(" valid: %s\n", tmp);

		bitArrayToStr(node.leaf_mask, tmp);
		printf(" leaf:  %s\n", tmp);

		printf(" children: %hu", node.children_ptr);
		printf("\n}\n");
	}
	*/
	return octree;
}

