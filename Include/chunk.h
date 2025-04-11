#pragma once

#include <cstdint>
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

	uint32_t X,Y,Z;
};

} //namespace vxl
