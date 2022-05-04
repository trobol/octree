#ifndef _OCTREE_BUILDER_H
#define _OCTREE_BUILDER_H

#include "octree.h"

// 
class OctreeBuilder {
public:
    OctreeBuilder(uint32_t depth) : m_depth{depth} {
        nodeLevels.resize(depth+1);
        std::vector<OTNode>& level = nodeLevels[depth];
        level.resize(1);
        level[0].children_ptr = 1 << 1; // account for 
        level[0].valid_mask = 0;
        level[0].leaf_mask = 0;
    }
    uint32_t m_size; // 2 ^ m_depth
    uint32_t m_depth; // number of levels of octree
	uint32_t m_alloc_size;
    
    // parents of bottom level
    std::vector<std::vector<OTNode>> nodeLevels;

    uint32_t setNode(int target_x, int target_y, int target_z);

    
};


#endif