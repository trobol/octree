#include "vox_file.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <iomanip>

#define ASSERT_HEADER(str)                                             \
	if (!checkHeader(file, str))                                       \
	{                                                                  \
		std::cout << path << " was not a valid vox file" << std::endl; \
	}

bool checkHeader(FILE* file, const char header[4])
{
	char input[4];
	fread(input, sizeof(input[0]), 4, file);
	return std::strcmp(input, header);
}

int readInt(FILE* file)
{

	int32_t num;
	fread(&num, 4, 1, file);
	union {
		int8_t b[4];
		int n;
	};

	b[0] = (int8_t)(num >> 0u);
	b[1] = (int8_t)(num >> 8u);
	b[2] = (int8_t)(num >> 16u);
	b[3] = (int8_t)(num >> 24u);

	return n;
}

int readUInt(FILE* file)
{

	int32_t num;
	fread(&num, 4, 1, file);
	union {
		uint8_t b[4];
		uint32_t n;
	};

	b[0] = (uint8_t)(num >> 0u);
	b[1] = (uint8_t)(num >> 8u);
	b[2] = (uint8_t)(num >> 16u);
	b[3] = (uint8_t)(num >> 24u);

	return n;
}

void VoxFile::load(std::string path)
{
	FILE* file = std::fopen(path.c_str(), "rb");
	if (!file)
	{
		std::cout << "File " << path << " was not found" << std::endl;
		throw;
	}
	
	//VOX followed by a space
	ASSERT_HEADER("VOX ");

	std::fread(&mVersion, 4, 1, file);

	ASSERT_HEADER("MAIN");
	int chunkSize = readInt(file);
	int chunkCount = readInt(file);


	ASSERT_HEADER("SIZE");
	int sizeSize = readInt(file);
	int sizeChunks = readInt(file);

	mSize.x = readInt(file);
	mSize.z = readInt(file); //the z axis is up and down for .vox
	mSize.y = readInt(file);

	ASSERT_HEADER("XYZI");
	int xyziSize = readInt(file);
	int xyziChunks = readInt(file);

	mNumVoxels = readInt(file);

	mVoxels = new Voxel[mNumVoxels];

	if (!mVoxels)
		std::cout << "Failed to create array" << std::endl;

	int result = fread(mVoxels, sizeof(Voxel), mNumVoxels, file);
	if (result != mNumVoxels) { std::cout << "Didnt read all the voxels" << std::endl; }


	char input[4];
	fread(input, sizeof(input[0]), 4, file);
	std::cout << "HEADER: " << input << '\n'
		<< std::endl;
	if (std::strcmp(input, "RGBA"))
	{
	}

	std::fclose(file);
}

VoxFile::~VoxFile()
{
	delete[] mVoxels;
}