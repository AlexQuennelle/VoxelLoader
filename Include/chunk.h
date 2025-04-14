#pragma once

#include <cstdint>
#include <vector>
namespace vxl
{

class Chunk
{
	public:
	Chunk(uint32_t childrenOffset, char* bytes);
};

class SizeChunk : public Chunk
{
	public:
	SizeChunk(uint32_t childrenOffset, char* bytes);

	uint32_t X, Y, Z;
};

struct Vector4Int
{
	uint8_t X, Y, Z, I;
};

class XYZIChunk : public Chunk
{
	public:
	XYZIChunk(uint32_t childrenOffset, char* bytes);

	std::vector<Vector4Int> voxels;
};

class PackChunk : public Chunk
{
	public:
	PackChunk(uint32_t childrenOffset, char* bytes);
};

} //namespace vxl
