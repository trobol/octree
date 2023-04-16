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

#include <chrono>

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
		uint32_t parent_index;
		vec3 parent_pos; 
		{
			NodeStackEntry& entry = traversal_stack.top();
			child_offset = entry.child_offset;
			parent_index = entry.index;
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
			c.color = random_colors[parent_index % 256];
			c.size = size;
			traversal_stack.pop();
			size = size * 2.0;
			current_depth++;

			elements.push_back(c);
			continue;
		}
		
		uint32_t parent = m_array[parent_index];
		

		uint32_t children_ptr = parent >> 17;
		//if (parent.children_far) children_ptr = m_farpointers[children_ptr];
		uint32_t child_index = parent_index + children_ptr + child_offset;
		
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

		const uint32_t child_mask = 1 << child_offset;
		const uint32_t valid_mask = parent >> 8;
		// isn't valid node
		if ((valid_mask & child_mask) == 0) continue;
		// is leaf node
		if ((parent & child_mask) != 0) {
			Cube c;
			c.pos = pos + vec3(1.0, 1.0, 1.0);
			c.size = half_size;
			c.color = random_colors[child_index % 256];
			leafElements.push_back(c);
		} else {
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
		vec3 color = vec3(color32split[0], color32split[1], color32split[2]) * ( 1.0f / 255.0f );

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
		uint32_t node = octree.m_array[i];
		char tmp[9];
		bitArrayToStr(node>>8, tmp);
		printf(" valid: %s\n", tmp);

		bitArrayToStr(node, tmp);
		printf(" leaf:  %s\n", tmp);

		printf(" children: %hu", node >> 17);
		printf("\n}\n");
	}
	*/
	
	return octree;
}


struct Triangle {
	vec3 pos[3];
};



int planeBoxOverlap(vec3 normal,float d, float maxbox)
{

	vec3 vmin,vmax;

	//static const __m128 sign_mask = _mm_set1_ps(-0.f);
	//__m128 min;
	//__m128 max = normal.mm;



	vmax.x = copysignf(maxbox, normal.x);
	vmax.y = copysignf(maxbox, normal.y);
	vmax.z = copysignf(maxbox, normal.z);

	vmin = vmax * -1.0f;



	__m128 prod = _mm_mul_ps(normal.mm, vmin.mm);
	__m128 sum1 = _mm_hadd_ps(prod, prod);
    __m128 sum2 = _mm_hadd_ps(sum1, sum1);
	float dotmin = _mm_cvtss_f32(sum2);

	if(vec3::dot(normal,vmin)+d>0.0f) return 0;
	if(vec3::dot(normal,vmax)+d>=0.0f) return 1;

	return 0;
}

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)             \
    p0 = a*v0.y - b*v0.z;                    \
    p2 = a*v2.y - b*v2.z;                    \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)              \
    p0 = a*v0.y - b*v0.z;                    \
    p1 = a*v1.y - b*v1.z;                    \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)             \
    p0 = -a*v0.x + b*v0.z;                   \
    p2 = -a*v2.x + b*v2.z;                       \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)              \
    p0 = -a*v0.x + b*v0.z;                   \
    p1 = -a*v1.x + b*v1.z;                       \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)             \
    p1 = a*v1.x - b*v1.y;                    \
    p2 = a*v2.x - b*v2.y;                    \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)              \
    p0 = a*v0.x - b*v0.y;                \
    p1 = a*v1.x - b*v1.y;                    \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize + fb * boxhalfsize;   \
    if(min>rad || max<-rad) return 0;


// test the axis aligned bounding box against the bounds of the triangle
// expects the triangles points to be relative to the AABB's center
bool AABB_vs_triangleBounds_centered( vec3 v0, vec3 v1, vec3 v2, float boxhalfsize ) {

	// create the bounds of the triangle
	vec3 tri_min = vec3::min( v0, vec3::min(v1, v2));
	vec3 tri_max = vec3::max( v0, vec3::max(v1, v2));
	vec3 aabb_max( boxhalfsize, boxhalfsize, boxhalfsize );
	vec3 aabb_min( -boxhalfsize, -boxhalfsize, -boxhalfsize);

	// test if the triangle min is larger than the aabb max
	__m128 g_min = _mm_cmpgt_ps(tri_min.mm, aabb_max.mm);

	//or the triangle max is smaller than aabb min
	__m128 g_max = _mm_cmplt_ps(tri_max.mm, aabb_min.mm);

	return (_mm_movemask_ps(g_min) & 7) == 0 && (_mm_movemask_ps(g_max) & 7) == 0;
}

// src https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/
int triBoxOverlap(vec3 boxcenter, float boxhalfsize, const Triangle& tri)
{

  /*    use separating axis theorem to test overlap between triangle and box */
  /*    need to test for overlap in these directions: */
  /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
  /*       we do not even need to test these) */
  /*    2) normal of the triangle */
  /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
  /*       this gives 3x3=9 more tests */
   vec3 v0,v1,v2;
   float min,max,d,p0,p1,p2,rad,fex,fey,fez;
   vec3 normal, e0,e1,e2;

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */

   v0.mm = _mm_sub_ps(tri.pos[0].mm, boxcenter.mm);
   v1.mm = _mm_sub_ps(tri.pos[1].mm, boxcenter.mm);
   v2.mm = _mm_sub_ps(tri.pos[2].mm, boxcenter.mm);


	  /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in X-direction */
   __m128 box = _mm_set1_ps(boxhalfsize);
   __m128 min_v = _mm_min_ps(v0.mm, _mm_min_ps(v1.mm, v2.mm));
   __m128 max_v = _mm_max_ps(v0.mm, _mm_max_ps(v1.mm, v2.mm));

   __m128 g_min = _mm_cmpgt_ps(min_v, box);
   __m128 g_max = _mm_cmplt_ps(max_v, _mm_mul_ps(_mm_set1_ps(-1.0f), box));

   if((_mm_movemask_ps(g_min) & 7) != 0 || (_mm_movemask_ps(g_max) & 7) != 0) return 0;
	return 1;

   /* compute triangle edges */
   //e0 = v1 - v0;      /* tri edge 0 */
   //e1 = v2 - v1;      /* tri edge 1 */
   //e2 = v0 - v2;      /* tri edge 2 */

	e0.mm = _mm_sub_ps(v1.mm, v0.mm);
	e1.mm = _mm_sub_ps(v2.mm, v1.mm);
	e2.mm = _mm_sub_ps(v0.mm, v2.mm);

	static const __m128 sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   vec3 fe;




	// flip 


	//   a, b, fa, fb
   // AXISTEST_X01 e0.z, e0.y, fez, fey 
//   p0 = e0.z*v0.y - e0.y*v0.z;                    // cross (e0, v0).x
//   p2 = e0.z*v2.y - e0.y*v2.z;                    // cross (e0, v2).x
//   if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} 
//   rad = fez * boxhalfsize + fey * boxhalfsize;   
//   if(min>rad || max<-rad) return 0;
//
//	//AXISTEST_Y02 e0.z, e0.x, fez, fex
//	p0 = -e0.z*v0.x + e0.x*v0.z;                   // cross (-e0, v0).y
//   p2 = -e0.z*v2.x + e0.x*v2.z;                   // cross (-e0, v2).y
//   if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} 
//   rad = fez * boxhalfsize + fex * boxhalfsize;   
//   if(min>rad || max<-rad) return 0;
//
//
//	// AXISTEST_Z12 e0.y, e0.x, fey, fex
//	p1 = e0.y*v1.x - e0.x*v1.y;                      // cross(e0, v1)
//   p2 = e0.y*v2.x - e0.x*v2.y;                    	 // cross(e0, v2)
//   if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} 
//	
//   rad = fey * boxhalfsize + fex * boxhalfsize;   
//   if(min>rad || max<-rad) return 0;
//	
 fe.mm = _mm_andnot_ps(sign_mask, e0.mm);
   AXISTEST_X01(e0.z, e0.y, fe.z, fe.y);
   AXISTEST_Y02(e0.z, e0.x, fe.z, fe.x);
   AXISTEST_Z12(e0.y, e0.x, fe.y, fe.x);

     fe.mm = _mm_andnot_ps(sign_mask, e1.mm);
   AXISTEST_X01(e1.z, e1.y, fe.z, fe.y); // cross (e1, v0).x, cross(e1, v2).x
   AXISTEST_Y02(e1.z, e1.x, fe.z, fe.x); // cross (-e1, v0).y, cross(-e1, v2).x
   AXISTEST_Z0(e1.y, e1.x, fe.y, fe.x); // cross (e1, v0).z, cross(e1, v1).x

 fe.mm = _mm_andnot_ps(sign_mask, e2.mm);
   AXISTEST_X2(e2.z, e2.y, fe.z, fe.y); // cross(e2, v0).x, cross(e2, v1).x
   AXISTEST_Y1(e2.z, e2.x, fe.z, fe.x);
   AXISTEST_Z12(e2.y, e2.x, fe.y, fe.x);



   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */

   normal = vec3::cross(e0, e1);
   d=-vec3::dot(normal, v0);
   if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;

   return 1;   /* box and triangle overlaps */
}


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



int triBoxOverlapFaster(vec3 boxcenter, float boxhalfsize, const Triangle& tri) {
	

  /*    use separating axis theorem to test overlap between triangle and box */

  /*    need to test for overlap in these directions: */

  /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */

  /*       we do not even need to test these) */

  /*    2) normal of the triangle */

  /*    3) crossproduct(edge from tri, {x,y,z}-directin) */

  /*       this gives 3x3=9 more tests */

	vec3 v0,v1,v2;
	float min,max,d,p0,p1,p2,rad,fex,fey,fez;
	vec3 normal, e0,e1,e2;

   /* This is the fastest branch on Sun */

   /* move everything so that the boxcenter is in (0,0,0) */

	v0 = tri.pos[0] - boxcenter;
	v1 = tri.pos[1] - boxcenter;
	v2 = tri.pos[2] - boxcenter;




   /* compute triangle edges */

	e0 = v1 - v0; /* tri edge 0 */
	e1 = v2 - v1; /* tri edge 1 */
	e2 = v0 - v2; /* tri edge 2 */

   /* Bullet 3:  */

   /*  test the 9 tests first (this was faster) */

   fex = fabsf(e0.x);
   fey = fabsf(e0.y);
   fez = fabsf(e0.z);

   vec3 fe = vec3::abs( e0 );

	// AXISTEST_X01 AXISTEST_X01(e0[Z], e0[Y], fez, fey);
	p0 = e0.z*v0.y - e0.y*v0.z;
    p2 = e0.z*v2.y - e0.y*v2.z;
	min = std::min( p0, p2 );
	max = std::max( p0, p2 );
    rad = fez * boxhalfsize + fey * boxhalfsize; 
    if(min>rad || max<-rad) return 0;

	// AXISTEST_Y02 AXISTEST_Y02(e0[Z], e0[X], fez, fex);
	p0 = -e0.z*v0.x + e0.x*v0.z;
    p2 = -e0.z*v2.x + e0.x*v2.z;
    min = std::min( p0, p2 );
	max = std::max( p0, p2 );
    rad = fez * boxhalfsize + fex * boxhalfsize; 
    if(min>rad || max<-rad) return 0;


	// AXISTEST_Z12 AXISTEST_Z12(e0[Y], e0[X], fey, fex);
	p1 = e0.y*v1.x - e0.x*v1.y;
    p2 = e0.y*v2.x - e0.x*v2.y;
    min = std::min( p0, p2 );
	max = std::max( p0, p2 );
    rad = fey * boxhalfsize + fex * boxhalfsize;
    if(min>rad || max<-rad) return 0;


/*
	{
	//
	// a * b + c * d   
	// b * d + f * h
	//  [ a c e g ]
	//  [ b d f h ]
	//  
	vec3 r0 = vec3( e0.z, -e0.z, e0.y );
	vec3 r2 = vec3( -e0.y, e0.x, -e0.x );

	vec3 a = vec3( v0.y, v0.x, v1.x );
	vec3 b = vec3( v0.z, v0.z, v1.y );
	vec3 s0 = r0 * a + r2 * b;

	vec3 c = vec3( v2.y, v2.x, v2.x ); 
	vec3 d = vec3( v2.z, v2.z, v2.y );
	vec3 s2 = r0 * c + r2 * d;

	vec3 vmin = vec3::min( s0, s2 );
	vec3 vmax = vec3::max( s0, s2 );

	vec3 fe0 = vec3( fe.z, fe.z, fe.y );
	vec3 fe1 = vec3( fe.y, fe.x, fe.x );
	vec3 vblockhalfsize = vec3( boxhalfsize, boxhalfsize, boxhalfsize );

	vec3 vrad = fe0 + fe1 * vblockhalfsize;

	//if (vmin.x>vrad.x || vmax.x<-vrad.x) return 0;
	//if (vmin.y>vrad.y || vmax.y<-vrad.y) return 0;
	//if (vmin.z>vrad.z || vmax.z<-vrad.z) return 0;
	
	}
*/
	fex = fabsf(e1.x);
	fey = fabsf(e1.y);
	fez = fabsf(e1.z);

	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0(e1.y, e1.x, fey, fex);



	fex = fabsf(e2.x);
	fey = fabsf(e2.y);
	fez = fabsf(e2.z);

	AXISTEST_X2(e2.z, e2.y, fez, fey);
	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);



   /* Bullet 1: */

   /*  first test overlap in the {x,y,z}-directions */

   /*  find min, max of the triangle each direction, and test for overlap in */

   /*  that direction -- this is equivalent to testing a minimal AABB around */

   /*  the triangle against the AABB */



   /* test in X-direction */



   FINDMINMAX(v0.x,v1.x,v2.x,min,max);

   if(min>boxhalfsize || max<-boxhalfsize) return 0;



   /* test in Y-direction */

   FINDMINMAX(v0.y,v1.y,v2.y,min,max);

   if(min>boxhalfsize || max<-boxhalfsize) return 0;



   /* test in Z-direction */

   FINDMINMAX(v0.z,v1.z,v2.z,min,max);

   if(min>boxhalfsize || max<-boxhalfsize) return 0;



   /* Bullet 2: */

   /*  test if the box intersects the plane of the triangle */

   /*  compute plane equation of triangle: normal*x+d=0 */

   normal = vec3::cross( e0, e1 );

   // -NJMP- (line removed here)
	  d=-vec3::dot(normal, v0);
   if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;	// -NJMP-



   return 1;   /* box and triangle overlaps */
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


void add_points( std::vector<vec3int>& points, vec3int pos, size_t size, const Triangle& tri) {
	float invsize = 1.0f / (float)size;
	vec3 boxcenter((float)pos.x*invsize, (float)pos.y*invsize, (float)pos.z*invsize);
	//printf("hit %i %i %i\n", pos.x, pos.y, pos.z);
	if ( triBoxOverlap( boxcenter, invsize * 0.5f, tri) == 0) {
		return;
	}

	

	if ( size == 1 ) {
		
		points.push_back( pos );
	} else {
		size = size / 2;
		int i_size = size;
		add_points( points, { pos.x + i_size, pos.y + i_size, pos.z + i_size}, size, tri);
		add_points( points, { pos.x - i_size, pos.y + i_size, pos.z + i_size}, size, tri);
		add_points( points, { pos.x + i_size, pos.y - i_size, pos.z + i_size}, size, tri);
		add_points( points, { pos.x + i_size, pos.y + i_size, pos.z - i_size}, size, tri);
		add_points( points, { pos.x - i_size, pos.y - i_size, pos.z + i_size}, size, tri);
		add_points( points, { pos.x + i_size, pos.y - i_size, pos.z - i_size}, size, tri);
		add_points( points, { pos.x - i_size, pos.y + i_size, pos.z - i_size}, size, tri);
		add_points( points, { pos.x - i_size, pos.y - i_size, pos.z - i_size}, size, tri);
	}
}

Octree Octree::fromMesh(std::vector<Face>& faces) {
	std::vector<Triangle> triangles;
	triangles.reserve(faces.size());
	for (const Face &face : faces) {
		triangles.push_back({face.vertices[0].pos, face.vertices[1].pos, face.vertices[2].pos});
	}

	// find the bounds of the object
	
	vec3 max = triangles[0].pos[0];
	vec3 min = triangles[0].pos[0];
	for (Triangle tri : triangles) {
		for (size_t i = 0; i < 3; i++) {
			max = vec3::max( tri.pos[i], max);
			min = vec3::min( tri.pos[i], min);
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
	/*
			float boxhalfsize = 1.0f / (float)size / 2.0f;
	for (size_t x = 0; x < size; x++) {
	for (size_t y = 0; y < size; y++)
	for (size_t z = 0; z < size; z++) {
	
		vec3int pos = {(int)(x), (int)(y), (int)(z)};
		vec3 boxcenter(pos.x/(float)size, pos.y/(float)size, pos.z/(float)size);
		for(size_t i = 0; i < triangles.size(); i++) {
			if (triBoxOverlap(boxcenter, boxhalfsize, triangles[i])) {
				points.push_back(pos);
				break;
			}
		}
	}
	*/

  	auto start = std::chrono::high_resolution_clock::now();
	auto step_start = std::chrono::high_resolution_clock::now();
	float invsize = 1.0f / (float)size;
	vec3int pos = {(int)(size/2), (int)(size/2), (int)(size/2)};
	for(const Triangle& tri : triangles) {
		__m128 axis[3];
		//calculateAxis(axis, tri);
		//std::vector<int> offsets(step_count * step_count * step_count);
		//const float boxhalfsize = 1.0f / (float)size / 2.0f;
	

		//add_points( points, pos, size, tri);
			
		//float boxhalfsize = invsize * 0.5f;
		float boxhalfsize = 1.0f / (float)size / 2.0f;
		
		for (size_t x = 0; x < size; x++) {
		for (size_t y = 0; y < size; y++)
		for (size_t z = 0; z < size; z++) {
		
			vec3int pos = {(int)(x), (int)(y), (int)(z)};
			//vec3 boxcenter(pos.x*invsize, pos.y*invsize, pos.z*invsize);
			vec3 boxcenter(pos.x/(float)size, pos.y/(float)size, pos.z/(float)size);

			if (triBoxOverlap(boxcenter, boxhalfsize, tri)) {
				points.push_back(pos);
				break;
			}
			
		}

		}
	
	i++;
	if (i % 100 == 0) {
		auto step = std::chrono::high_resolution_clock::now();
		auto diff = step - step_start;
		int64_t ms = std::chrono::duration_cast< std::chrono::milliseconds >( diff ).count();
		printf("%6u/%zu  %2.1f%% %6ims\n", i, triangles.size(), i/(float)triangles.size()*100.0f, ms);
		step_start = std::chrono::high_resolution_clock::now();
	}

	}

	auto end = std::chrono::high_resolution_clock::now();

	for (vec3int p : points) {
		result.setNode(p.x, p.y, p.z, (uint16_t)-1);
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

	vec3 center = pos + vec3::one * size * 0.5f;

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
