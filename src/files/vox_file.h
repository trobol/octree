#ifndef _FILE_TYPES_VOX_FILE_H
#define _FILE_TYPES_VOX_FILE_H
#include <string>
#include <vector>


#include "../math/vec3int.h"

constexpr int32_t strToInt(const char[4]);

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

	enum CHUNK_TYPE : int32_t {
		VOX = 542658390,
		MAIN = 1313423693,
		SIZE = 1163544915,
		XYZI = 1230657880,
		RGBA = 1094862674,
		UNKNOWN = 0
	};
	struct Chunk {
		virtual void read(FILE* file);
	};
	struct Chunk_MAIN : Chunk {
		const CHUNK_TYPE type = MAIN;
		std::vector<Chunk*> children;
		void read(FILE* file);
	};
	struct Chunk_SIZE : Chunk {
		const CHUNK_TYPE type = SIZE;
		void read(FILE* file);
	};
	struct Chunk_XYZI : Chunk {
		const CHUNK_TYPE type = XYZI;
		void read(FILE* file);
	};
	struct Chunk_RGBA : Chunk {
		const CHUNK_TYPE type = RGBA;
		void read(FILE* file);
	};
	struct Chunk_UNKNOWN : Chunk {
		const CHUNK_TYPE type = UNKNOWN;
		void read(FILE* file);
	};
};

#endif