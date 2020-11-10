#ifndef _OCTREE_DEFINES_H
#define _OCTREE_DEFINES_H

#include <octree/math/vec3.h>
#define ELEMENTS_PER_CUBE 8
#define INDICES_PER_BRANCH 24

#define INDICES_PER_LEAF 36

const vec3 CUBE_POINTS[ELEMENTS_PER_CUBE] = {
	{0, 0, 0},
	{0, 0, 1},
	{0, 1, 0},
	{0, 1, 1},
	{1, 0, 0},
	{1, 0, 1},
	{1, 1, 0},
	{1, 1, 1},
};


const int BRANCH_INDICES[INDICES_PER_BRANCH] = {
	0, 1,
	1, 5,
	5, 4,
	4, 0,

	2, 3,
	3, 7,
	7, 6,
	6, 2,

	2, 0,
	3, 1,
	7, 5,
	6, 4};
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
const unsigned int LEAF_INDICES[INDICES_PER_LEAF] = {
	0, 1, 2,
	3, 1, 2,

	0, 2, 4,
	6, 2, 4,

	2, 3, 6,
	7, 3, 6,

	5, 4, 7,
	6, 4, 7,

	5, 1, 7,
	3, 1, 7,

	1, 0, 5,
	4, 0, 5};

#endif