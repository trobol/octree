#ifndef _FILE_TYPES_VOX_FILE_H
#define _FILE_TYPES_VOX_FILE_H
#include <string>

#include "../math/vec3int.h"

class VoxFile
{
private:

	struct Voxel
	{
		char x, z, y;
		char colorIndex;
	};
	int mVersion;
	vec3int mSize;

	unsigned int mNumVoxels;

public:
	Voxel* mVoxels;
	~VoxFile();
	void load(std::string path);
	vec3int getSize() { return mSize; };
	int getNumVoxels() { return mNumVoxels; };
};

#endif
