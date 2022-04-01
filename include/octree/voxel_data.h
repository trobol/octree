#ifndef _OCTREE_VOXELDATA_H
#define _OCTREE_VOXELDATA_H
#include <vector>
#include <stdint.h>

#include "math/vec3int.h"

class VoxelData {
	// storage is x, y, z
	std::vector<uint8_t> m_data;
	uint32_t m_size;
public:


	VoxelData(uint32_t size);
	
	void set(vec3int pos) {
		uint32_t x_size = m_size / 8;
		if (m_size % 8 > 0) x_size++;
		uint32_t x_index = pos.x / 8;
		uint32_t index = (pos.z * m_size * m_size * x_size) + (pos.y * m_size * x_size) + x_index;
	}
	bool get(vec3int pos);

}

#endif