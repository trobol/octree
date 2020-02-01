#include "vox_file.h"

#include <fstream>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cstdio>

#define ASSERT_HEADER(str)                                             \
	if (!checkHeader(file, str))                                       \
	{                                                                  \
		std::cout << path << " was not a valid vox file" << std::endl; \
	}

bool checkHeader(FILE *file, const char header[4])
{
	char input[4];
	fread(input, sizeof(input[0]), 4, file);
	std::cout << "HEADER: " << input << std::endl;
	return std::strcmp(input, header);
}

int readInt(FILE *file)
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

void VoxFile::load(std::string path)
{
	FILE *file = std::fopen(path.c_str(), "r");
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
	//std::fseek(file, 8, SEEK_CUR); //skip chunk header

	/*
	if (checkHeader(file, "PACK"))
	{
		std::cout << "Files with multiple models are not supported yet" << std::endl;
		throw;
	}
	*/

	ASSERT_HEADER("SIZE");
	int sizeSize = readInt(file);
	int sizeChunks = readInt(file);
	//std::fseek(file, 8, SEEK_CUR); //skip chunk header

	mSize.x = readInt(file);
	mSize.y = readInt(file);
	mSize.z = readInt(file);

	ASSERT_HEADER("XYZI");
	int xyziSize = readInt(file);
	int xyziChunks = readInt(file);

	mNumVoxels = readInt(file);

	mVoxels = new Voxel[mNumVoxels];

	assert(mVoxels);
	char buf[4];
	for (int i = 0; i < mNumVoxels; i++)
	{
		fread(buf, 4, 1, file);
		mVoxels[i].pos.x = buf[0];
		mVoxels[i].pos.y = buf[1];
		mVoxels[i].pos.z = buf[2];
		mVoxels[i].colorIndex = buf[3];
	}

	std::fclose(file);
}

VoxFile::~VoxFile()
{
	delete[] mVoxels;
}