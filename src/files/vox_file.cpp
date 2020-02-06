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


int32_t getHeader(FILE* file) {
	union {
		int8_t input_char[4] = { 0, 0, 0, 0 };
		int32_t input_int;
	};
	fread(input_char, sizeof(input_char[0]), 4, file);
	std::cout << input_int << " " << input_char << std::endl;
	return input_int;
}

void VoxFile::Chunk_MAIN::read(FILE* file) {

}

void VoxFile::Chunk_SIZE::read(FILE* file) {

}

void VoxFile::Chunk_XYZI::read(FILE* file) {

}

void VoxFile::Chunk_RGBA::read(FILE* file) {

}

void VoxFile::Chunk_UNKNOWN::read(FILE* file) {

}


bool checkHeader(FILE* file, const char header[4])
{
	int32_t header_int = *((int32_t*)header);



	return getHeader(file) == header_int;
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

	while (feof(file)) {
		CHUNK_TYPE type = (CHUNK_TYPE)getHeader(file);
		Chunk* chunk;
		switch (type)
		{
		case MAIN:
			chunk = new Chunk_MAIN();
			break;
		case SIZE:
			chunk = new Chunk_SIZE();
			break;
		case XYZI:
			chunk = new Chunk_XYZI();
			break;
		case RGBA:
			chunk = new Chunk_RGBA();
			break;
		default:
			break;
		}
	}
	std::fclose(file);
}

VoxFile::~VoxFile()
{
	delete[] mVoxels;
}