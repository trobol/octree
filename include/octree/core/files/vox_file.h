#ifndef _FILE_TYPES_VOX_FILE_H
#define _FILE_TYPES_VOX_FILE_H
#include <string>
#include <vector>


#include <octree/math/vec3int.h>


struct Bounds3d {
	vec3int min, max;

	vec3int size();

	// not the true center because its an int
	vec3int center();
};

class VoxFile
{
private:
	static uint32_t DEFAULT_VOX_PALETTE[256];


	uint32_t* mPalette = nullptr;
	struct Voxel
	{
		char x, z, y;
		unsigned char colorIndex;
	};
	int mVersion;
	vec3int mSize;
	//Bounds3d bounds;

	void readChunk_SIZE(FILE* file, int size);
	void readChunk_XYZI(FILE* file, int size);
	void readChunk_RGBA(FILE* file, int size);
	void readChunk_PACK(FILE* file, int size);
public:
	std::vector<Voxel> mVoxels;

	~VoxFile();
	void load(std::string path);
	vec3int getSize() { return mSize; };
	size_t getNumVoxels() { return mVoxels.size(); };
	unsigned int* getPalette() { if (mPalette != nullptr) return mPalette; else return (unsigned int*)DEFAULT_VOX_PALETTE; };


	enum CHUNK_TYPE : int32_t {
		VOX = 542658390,
		MAIN = 1313423693,
		SIZE = 1163544915,
		XYZI = 1230657880,
		RGBA = 1094862674,
		UNKNOWN = 0
	};




};




#endif