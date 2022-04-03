#include <octree/octree_builder.h>


uint32_t OctreeBuilder::setNode(int target_x, int target_y, int target_z)
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

    uint32_t current_depth = m_depth; 

	while(current_depth > 0)
	{
	
		parent_index = current_index;

        uint32_t current_size = 1 << current_depth;
        uint32_t next_depth = current_depth-1;
		// get half size
		int half_size = 1 << next_depth;

		int center_x = x + half_size;
		int center_y = y + half_size;
		int center_z = z + half_size;

		// child positions
		int pos_x[8] = {x + half_size, x, x + half_size, x, x + half_size, x, x + half_size, x};
		int pos_y[8] = {y + half_size, y + half_size, y, y, y + half_size, y + half_size, y, y};
		int pos_z[8] = {z + half_size, z + half_size, z + half_size, z + half_size, z, z, z, z};

		

		// see child offset comment above
		child_offset = ((target_z < center_z) << 2) | ((target_y < center_y) << 1) | (target_x < center_x);
		
        std::vector<OTNode>& parent_level = nodeLevels[current_depth];
        std::vector<OTNode>& child_level = nodeLevels[next_depth];

        // no valid children
		// we are going to make some children valid, so allocate them
		if (!parent_level[current_index].valid_mask) {
			uint32_t ptr = (uint32_t)child_level.size() - current_index;
            if (ptr > (1 << 14)) puts("ERROR: node index will overflow");
			parent_level[current_index].children_ptr = ptr;
			child_level.resize(child_level.size() + 8); 
		}
		// assume that parent node is already a valid node
		// this means all members are valid values and has space designated for children, even if it has no valid children
		bool child_valid;
		{ // prevents parent reference usage after array resize
			OTNode& parent = parent_level[parent_index];
			// TODO: mask check should be a function
			// is the child we want to work on next valid?
			child_valid = parent.valid_mask & (1 << child_offset);
			// we are gonna make the child valid if its not
			parent.valid_mask |= 1 << child_offset;

			uint32_t children_ptr = parent.children_ptr;
			current_index = children_ptr;
		}

		x = pos_x[child_offset];
		y = pos_y[child_offset];
		z = pos_z[child_offset];
		
		//printf("halfsize: %5i\n", half_size);
        printf("(%2i, %2i, %2i) [%i]\n", x, y, z, half_size);
		current_depth--;
	}

	//printf("(%5i, %5i, %5i) (%5i, %5i, %5i) [%5i]\n ", x, y, z, target_x, target_y, target_z, current_depth);

	nodeLevels[0][parent_index].leaf_mask |= 1 << child_offset;
    
	if (target_x != x || target_y != y || target_z != z) puts("BUILDER ERROR: target did not match destination in octree");

	return current_index;
}
