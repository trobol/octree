#include <octree/octree.h>
#include <stack>

#include <algorithm>
#include <math.h>
#include <octree/math/octree_math.h>
#include <octree/math/vec2.h>
#include <octree/octree_builder.h>
#include <algorithm>
#include <pmmintrin.h>
#include <smmintrin.h>

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

	m_array[0] = 0;

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
		uint8_t child_offset;
		vec3 pos;
	};
	std::stack<NodeStackEntry> traversal_stack;
	traversal_stack.push({ 0, 0, vec3(0.0f, 0.0f, 0.0f) });



	uint32_t current_depth = m_depth;

	//uint32_t current_size = m_size;
	float size = 1.0; 
	while (!traversal_stack.empty()) {

		uint8_t child_offset;
		uint32_t index;
		vec3 parent_pos; 
		{
			NodeStackEntry& entry = traversal_stack.top();
			child_offset = entry.child_offset;
			index = entry.index;
			entry.child_offset++;
			parent_pos = entry.pos;
			//current_depth = entry.depth;
		}
		float half_size = size * 0.5;
		
		// done with this node (and all its children)
		if (child_offset > 7) {

			// draw branch
			Cube c;
			c.pos = parent_pos + vec3(1.0, 1.0, 1.0);
			c.color = random_colors[index % 256];
			c.size = size;
			traversal_stack.pop();
			size = size * 2.0;
			current_depth++;

			elements.push_back(c);
			continue;
		}
		
		uint32_t parent = m_array[index];
		

		uint32_t children_ptr = parent >> 17;
		//if (parent.children_far) children_ptr = m_farpointers[children_ptr];
		uint32_t child_index = index + children_ptr + child_offset;
		
		vec3 pos_table[8] = {
			{0.0f, 0.0f, 0.0f },
			{1.0f, 0.0f, 0.0f },
			{0.0f, 1.0f, 0.0f },
			{1.0f, 1.0f, 0.0f },
			{0.0f, 0.0f, 1.0f },
			{1.0f, 0.0f, 1.0f },
			{0.0f, 1.0f, 1.0f },
			{1.0f, 1.0f, 1.0f },
		};

		vec3 pos = parent_pos + pos_table[child_offset] * half_size;	

		// is valid node
		if (!((parent >> 8) & (1 << child_offset))) continue;
		// is leaf node
		if (parent & (1 << child_offset)) {
			Cube c;
			c.pos = pos + vec3(1.0, 1.0, 1.0);
			c.size = half_size;
			c.color = random_colors[child_index % 256];
			leafElements.push_back(c);
			continue;
		} 

		{
			// if non-leaf, add to elements and push to traversal_stack
			uint8_t of = child_offset;	
			parent_pos = pos;
			current_depth--;
			size = half_size;
			traversal_stack.push({child_index, 0, pos});
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

uint32_t Octree::setNode(int target_x, int target_y, int target_z, uint32_t value, uint16_t min_depth)
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
		int pos_x[8] = {x, x + half_size, x, x + half_size, x, x + half_size, x, x + half_size};
		int pos_y[8] = {y, y, y + half_size, y + half_size, y, y, y + half_size, y + half_size};
		int pos_z[8] = {z, z, z, z, z + half_size, z + half_size, z + half_size, z + half_size};
		
		

		// see child offset comment above
		child_offset = ((target_z >= center_z) << 2) | ((target_y >= center_y) << 1) | (target_x >= center_x);
		
		
		// no valid children
		// we are going to make some children valid, so allocate them
		if ((m_array[current_index] & 0xFF00) == 0) {
			uint32_t ptr = (uint32_t)m_array.size() - current_index;
			if (ptr > (0x7FFF)) puts("ERROR: node index will overflow");
		
			m_array[current_index] |= ptr << 17;
			m_array.resize(m_array.size() + 8, {});
		}
		// assume that parent node is already a valid node
		// this means all members are valid values and has space designated for children, even if it has no valid children
		bool child_valid;
		{ // prevents parent reference usage after array resize
			uint32_t& parent = m_array[parent_index];
			uint32_t children_ptr = parent >> 17;
			// TODO: mask check should be a function
			// is the child we want to work on next valid?
			uint32_t valid_mask = 1 << child_offset; 
			child_valid = (parent >> 8) & valid_mask;
			// we are gonna make the child valid if its not
			parent |= valid_mask << 8; // assign to valid mask
			if (half_size == 1) parent |= valid_mask ; // assign to leaf mask, aka set as leaf
			//if (parent.children_far) children_ptr = m_farpointers[children_ptr];
			current_index = parent_index + children_ptr + child_offset;
		}

		
		x = pos_x[child_offset];
		y = pos_y[child_offset];
		z = pos_z[child_offset];
		
		//printf("%2i,%2i x: %5i y: %5i z: %5i\n", half_size, child_offset, x, y, z);

		current_size = half_size;
	}

	
	//if (min_depth == 0)
	//	m_array[parent_index].leaf_mask = 0;
	m_array[current_index] = value;
	if (target_x != x || target_y != y || target_z != z) printf("ERROR: target did not match destination in octree, x: %5i y: %5i z: %5i\n", target_x, target_y, target_z);

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
	setNode(pos.x, pos.y, pos.z, 0);
	
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

	uint32_t* palette = file.getPalette();
	//load
	for (int i = 0; i < file.getNumVoxels(); i++)
	{
		color32 = palette[file.mVoxels[i].colorIndex - 1];
		vec3 color = vec3(color32split[0], color32split[1], color32split[2]) / 255;

		//if (file.mVoxels[i].x > 50 || file.mVoxels[i].y > 50 || file.mVoxels[i].z > 50) continue;
		//dont forget to convert to correct space
		
		octree.setNode(file.mVoxels[i].x, file.mVoxels[i].y, file.mVoxels[i].z, color32);
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


struct Triangle {
	vec3 pos[3];
};


static const __m128 sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
bool IntersectsTriangleAabbSat(__m128 a[3], __m128 v0[3], __m128 v1[3], __m128 v2[3], float halfsize) {
	
	// dot product of point and axis
	__m128 prodx, prody, prodz;
	__m128 dot0, dot1, dot2;


	prodx = _mm_mul_ps(a[0], v0[0]);
	prody = _mm_mul_ps(a[1], v0[1]);
	prodz = _mm_mul_ps(a[2], v0[2]);
	dot0 = _mm_add_ps(prodx, _mm_add_ps(prody, prodz));

	prodx = _mm_mul_ps(a[0], v1[0]);
	prody = _mm_mul_ps(a[1], v1[1]);
	prodz = _mm_mul_ps(a[2], v1[2]);
	dot1 = _mm_add_ps(prodx, _mm_add_ps(prody, prodz));

	prodx = _mm_mul_ps(a[0], v2[0]);
	prody = _mm_mul_ps(a[1], v2[1]);
	prodz = _mm_mul_ps(a[2], v2[2]);
	dot2 = _mm_add_ps(prodx, _mm_add_ps(prody, prodz));

	__m128 min = _mm_min_ps(dot0, _mm_min_ps(dot1, dot2));
	__m128 max = _mm_max_ps(dot0, _mm_max_ps(dot1, dot2));


	// calculate r
	
	__m128 r = _mm_add_ps(a[0], _mm_add_ps(a[1], a[2]));
	
	r = _mm_andnot_ps(sign_mask, r); // abs(r)
	r = _mm_mul_ps(_mm_set1_ps(halfsize), r);

	__m128 nmax = _mm_mul_ps(max, _mm_set1_ps(-1.0f));

	__m128 res = _mm_cmpgt_ps(_mm_max_ps(nmax, min), r);
	if (_mm_movemask_ps(res) != 0) return false;

	return true;
}


void calculateAxis(__m128 axis_out[3], const Triangle& tri) {
	// pack each components into its own val
	__m128 x, y, z;

	__m128 a = tri.pos[0].mm; // a
	__m128 b = tri.pos[1].mm; // b
	__m128 c = tri.pos[2].mm; // c
	
	__m128 ab = _mm_sub_ps(b, a); // 0
	__m128 bc = _mm_sub_ps(c, b); // 1
	__m128 ca = _mm_sub_ps(a, c); // 2
	__m128 x0y0x1y1 = _mm_shuffle_ps(ab, bc, _MM_SHUFFLE(1, 0, 1, 0));
	__m128 z0w0z1w1 = _mm_shuffle_ps(ab, bc, _MM_SHUFFLE(3, 2, 3, 2));

	x = _mm_shuffle_ps(x0y0x1y1, ca, _MM_SHUFFLE(3, 0, 2, 0));
	y = _mm_shuffle_ps(x0y0x1y1, ca, _MM_SHUFFLE(3, 1, 3, 1));
	z = _mm_shuffle_ps(z0w0z1w1, ca, _MM_SHUFFLE(3, 2, 2, 0));
	
	// square components
	__m128 sqrx = _mm_mul_ps(x, x);
	__m128 sqry = _mm_mul_ps(y, y);
	__m128 sqrz = _mm_mul_ps(z, z);

	__m128 sum = _mm_add_ps(sqrx, _mm_add_ps(sqry, sqrz));
	__m128 rsqrt = _mm_rsqrt_ps(sum);
	
	// normalize
	axis_out[0] = _mm_mul_ps(x, rsqrt);
	axis_out[1] = _mm_mul_ps(y, rsqrt);
	axis_out[2] = _mm_mul_ps(z, rsqrt);

}

bool IntersectsTriangleAabb(vec3 boxcenter, float boxhalfsize, __m128 axis_in[3], const Triangle& tri) {

	__m128 a = _mm_sub_ps(tri.pos[0].mm, boxcenter.mm); // a
	__m128 b = _mm_sub_ps(tri.pos[1].mm, boxcenter.mm); // b
	__m128 c = _mm_sub_ps(tri.pos[2].mm, boxcenter.mm); // c



__m128 x, y, z;
	
	x = axis_in[0];
	y = axis_in[1];
	z = axis_in[2];

/*
	__m128 neg = _mm_set_ps(0.0f, -1.0f, -1.0f, -1.0f);
	__m128 nab = _mm_mul_ps(ab, neg);
	__m128 nbc = _mm_mul_ps(bc, neg);
	__m128 nca = _mm_mul_ps(ca, neg);


    //Cross ab, bc, and ca with (1, 0, 0)
    __m128 a00 = _mm_shuffle_ps(nab, ab, _MM_SHUFFLE(3, 2, 1, 3)); // float3(0.0, -ab.z, ab.y);
    __m128 a01 = _mm_shuffle_ps(nbc, bc, _MM_SHUFFLE(3, 2, 1, 3)); // float3(0.0, -bc.z, bc.y);
    __m128 a02 = _mm_shuffle_ps(nca, ca, _MM_SHUFFLE(3, 2, 1, 3)); // float3(0.0, -ca.z, ca.y);

    //Cross ab, bc, and ca with (0, 1, 0)
    __m128 a10 = _mm_shuffle_ps(ab, nab, _MM_SHUFFLE(2, 3, 0, 3)); // float3(ab.z, 0.0, -ab.x);
    __m128 a11 = _mm_shuffle_ps(bc, nbc, _MM_SHUFFLE(2, 3, 0, 3)); // float3(bc.z, 0.0, -bc.x);
    __m128 a12 = _mm_shuffle_ps(ca, nca, _MM_SHUFFLE(2, 3, 0, 3)); // float3(ca.z, 0.0, -ca.x);

    //Cross ab, bc, and ca with (0, 0, 1)
    __m128 a20 = _mm_shuffle_ps(ab, nab, _MM_SHUFFLE(2, 3, 0, 3)); // float3(-ab.y, ab.x, 0.0);
    __m128 a21;// = // float3(-bc.y, bc.x, 0.0);
    __m128 a22;// = // float3(-ca.y, ca.x, 0.0);
*/
	
	__m128 v0[3];
	__m128 v1[3];
	__m128 v2[3];

	
	v0[0] = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0));
	v0[1] = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 1, 1, 1));
	v0[2] = _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 2, 2, 2));

	v1[0] = _mm_shuffle_ps(b, b, _MM_SHUFFLE(0, 0, 0, 0));
	v1[1] = _mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 1, 1, 1));
	v1[2] = _mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 2, 2, 2));

	v2[0] = _mm_shuffle_ps(c, c, _MM_SHUFFLE(0, 0, 0, 0));
	v2[1] = _mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 1, 1, 1));
	v2[2] = _mm_shuffle_ps(c, c, _MM_SHUFFLE(2, 2, 2, 2));


	__m128 neg1 = _mm_set1_ps(-1.0f);
	__m128 zero = _mm_set1_ps(0.0f);
	__m128 axis[3];
	// for now the 4th component (w) is 0
	axis[0] = zero; //   0, 0, 0,
	axis[1] = _mm_mul_ps(z, neg1); // -ab.z, -bc.z, -ca.z
	axis[2] = y; // ab.y, bc.y, ca.y
	if (!IntersectsTriangleAabbSat(axis, v0, v1, v2, boxhalfsize)) return false;

	axis[0] = z; //   0, 0, 0,
	axis[1] = zero; 
	axis[2] = _mm_mul_ps(x, neg1);
	if (!IntersectsTriangleAabbSat(axis, v0, v1, v2, boxhalfsize)) return false;

	axis[0] = _mm_mul_ps(y, neg1); //   0, 0, 0,
	axis[1] = x; // -ab.z, -bc.z, -ca.z
	axis[2] = zero; // ab.y, bc.y, ca.y
	if (!IntersectsTriangleAabbSat(axis, v0, v1, v2, boxhalfsize)) return false;




	// TESTS AGAINST BASIC AABB OF TRIANGLE
	// TODO: move it above for early return ?
	axis[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f); // 1, 0, 0,
	axis[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f); // 0, 1, 0
	axis[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f); // 0, 0, 1
	if (!IntersectsTriangleAabbSat(axis, v0, v1, v2, boxhalfsize)) return false;

	// TODO: test triangle normal, only 1
	// cross product of ab, bc
	vec3 av;
	vec3 bv;
	__m128 xxyy = _mm_shuffle_ps(x, y, _MM_SHUFFLE(2, 0, 2, 0));
	av.mm = _mm_shuffle_ps(xxyy, z, _MM_SHUFFLE(3, 0, 2, 0));
	bv.mm = _mm_shuffle_ps(xxyy, z, _MM_SHUFFLE(3, 2, 3, 1));


	
	//__m128 norm = vec3::cross(av, bv).mm;

	//axis[0] = _mm_shuffle_ps(norm, norm, _MM_SHUFFLE(0, 0, 0, 0)); // 1, 0, 0,
	//axis[1] = _mm_shuffle_ps(norm, norm, _MM_SHUFFLE(1, 1, 1, 1)); // 0, 1, 0
	//axis[2] = _mm_shuffle_ps(norm, norm, _MM_SHUFFLE(2, 2, 2, 2)); // 0, 0, 1
	//if (!IntersectsTriangleAabbSat(axis, v0, v1, v2, boxhalfsize)) return false;
/*
    if (
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a00) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a01) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a02) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a10) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a11) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a12) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a20) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a21) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a22) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(1, 0, 0)) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(0, 1, 0)) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(0, 0, 1)) ||
        !IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, cross(ab, bc))
    )
    {
        return false;
    }
*/
    return true;
}

bool contains_triangle(std::vector<vec3int>& points, size_t x, size_t y, size_t z, size_t size, size_t step_size, const Triangle& tri, __m128 axis[3]) {
	for (size_t xo = 0; xo < 2; xo++) {
	for (size_t yo = 0; yo < 2; yo++)
	for (size_t zo = 0; zo < 2; zo++) {
		vec3int pos = {(int)x+(int)(xo*step_size), (int)y+(int)(yo*step_size), (int)z+(int)(zo*step_size)};
		vec3 boxcenter(pos.x/(float)size, pos.y/(float)size, pos.z/(float)size);
		float boxhalfsize = (0.5f / (float)size) * (float)step_size;
		printf("%zu %f\n", step_size, boxhalfsize);
		if (IntersectsTriangleAabb(boxcenter, boxhalfsize, axis, tri)) {
			puts("hit");
			if (step_size == 1) {
				points.push_back(pos);
			} else {
				contains_triangle(points, pos.x, pos.y, pos.z, size, step_size / 2, tri, axis);
			}
		}
	}
	}

	return false;
}


Octree Octree::fromMesh(std::vector<Face>& faces) {
	std::vector<Triangle> triangles;
	triangles.reserve(faces.size());
	for (const Face &face : faces) {
		triangles.push_back({face.vertices[0].pos, face.vertices[1].pos, face.vertices[2].pos});
	}

	// find the bounds of the object
	
	vec3 max = {-10000, -10000, -10000};
	vec3 min = { 10000,  10000,  10000};
	for (Triangle tri : triangles) {
	for (size_t i = 0; i < 3; i++) {
		max.x = std::max(tri.pos[i].x, max.x);
		max.y = std::max(tri.pos[i].y, max.y);
		max.z = std::max(tri.pos[i].z, max.z);
		min.x = std::min(tri.pos[i].x, min.x);
		min.y = std::min(tri.pos[i].y, min.y);
		min.z = std::min(tri.pos[i].z, min.z);
	}
	}

	// change the object so it goes from 0->1
	float scale_inv = 1.0f / vec3::largest(max);
	for(Triangle& tri : triangles) {
	for (size_t i = 0; i < 3; i++) {
		tri.pos[i] -= min;
		tri.pos[i] *= scale_inv;
	}
	}
	size_t size = 64;
	size_t step_size = 16;
	size_t step_count = size/16;
	float half_step = step_size/2.0f;
	Octree result(size);
	
	vec3int offset = {0, 0, 0};
	std::vector<vec3int> points;
	unsigned int i = 0;
	for(const Triangle& tri : triangles) {
		__m128 axis[3];
		calculateAxis(axis, tri);
	//std::vector<int> offsets(step_count * step_count * step_count);
		//const float boxhalfsize = 1.0f / (float)size / 2.0f;

	
		//
		//}	
		float boxhalfsize = 1.0f / (float)size / 2.0f;
		for (size_t x = 0; x < size; x++) {
		for (size_t y = 0; y < size; y++)
		for (size_t z = 0; z < size; z++) {
		
			vec3int pos = {(int)(x), (int)(y), (int)(z)};
			vec3 boxcenter(pos.x/(float)size, pos.y/(float)size, pos.z/(float)size);

			if (IntersectsTriangleAabb(boxcenter, boxhalfsize, axis, tri)) {
				points.push_back(pos);
				break;
			}
			
		}
		//printf("x = %zu\n", x);
		}
	
	i++;
	if (i % 100 == 0)
		printf("%6u/%zu  %2.1f%%\n", i, triangles.size(), i/(float)triangles.size()*100.0f);

	}

	for (vec3int p : points) {
		result.setNode(p.x, p.y, p.z, (uint32_t)-1);
	}



	return result;
}


vec2 project_cube(vec3 dT, vec3 bT, vec3 pos, float size) {
	vec3 cube_min = vec3(size, size, size) + pos;
	vec3 cube_max = pos;


	float tc_min = vec3::largest(dT * cube_min + bT);
	float tc_max = vec3::smallest(dT * cube_max + bT);

	return vec2{tc_min, tc_max};
}

vec2 intersect(vec2 a, vec2 b) {
	return vec2(std::max(a.x, b.x), std::min(a.y, b.y));
}

uint8_t select_child(vec3 dT, vec3 bT, vec3 pos, float size, float t_min) {

	vec3 center = pos + vec3::one * size / 2.0;

	vec3 p = dT * center + bT;
	uint8_t idx;
	idx ^= (p.x > t_min) & 1;
	idx ^= (p.y > t_min) & 2;
	idx ^= (p.z > t_min) & 4;
	return idx;
}

vec3 child_cube( uint8_t index, vec3 pos, float scale) {
	const vec3 pos_lookup[8] = {
		{0.0f, 0.0f, 0.0f },
		{0.0f, 0.0f, 1.0f },
		{0.0f, 1.0f, 0.0f },
		{0.0f, 1.0f, 1.0f },
		{1.0f, 0.0f, 0.0f },
		{1.0f, 0.0f, 1.0f },
		{1.0f, 1.0f, 0.0f },
		{1.0f, 1.0f, 1.0f },
	};

	return pos + pos_lookup[index] * scale;
}
