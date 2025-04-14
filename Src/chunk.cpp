#include "chunk.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

namespace vxl
{
std::ostream& operator<<(std::ostream& os, Vector4Int vec)
{
	os << '(' << static_cast<uint32_t>(vec.X) << ", "
	   << static_cast<uint32_t>(vec.Y) << ", " << static_cast<uint32_t>(vec.Z)
	   << ", " << static_cast<uint32_t>(vec.I) << ')';
	return os;
}

Chunk::Chunk(uint32_t childrenOffset, char* bytes)
{
	std::cout << "New ";
	for (int i{0}; i < 4; i++)
	{
		std::cout << bytes[i];
	}
	std::cout << " chunk" << '\n';
}

SizeChunk::SizeChunk(uint32_t childrenOffset, char* bytes)
	: Chunk{childrenOffset, bytes}
{
	std::memcpy(&this->X, bytes + 12, 12);
	std::cout << '(' << this->X << ", " << this->Y << ", " << this->Z << ")\n";
}

XYZIChunk::XYZIChunk(uint32_t childrenOffset, char* bytes)
	: Chunk{childrenOffset, bytes}
{
	uint32_t voxelCount;
	std::memcpy(&voxelCount, bytes + 12, 4);
	this->voxels.reserve(voxelCount);
	for (uint32_t i{0}; i < voxelCount; i++)
	{
		this->voxels.push_back(
			{.X = static_cast<uint8_t>(*(bytes + 16 + (i * 4))),
			 .Y = static_cast<uint8_t>(*(bytes + 17 + (i * 4))),
			 .Z = static_cast<uint8_t>(*(bytes + 18 + (i * 4))),
			 .I = static_cast<uint8_t>(*(bytes + 19 + (i * 4)))});
	}
}

PackChunk::PackChunk(uint32_t childrenOffset, char* bytes)
	: Chunk{childrenOffset, bytes}
{}

} //namespace vxl
