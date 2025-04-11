#include "chunk.h"
#include <cstdint>
#include <iostream>

namespace vxl
{

Chunk::Chunk(uint32_t childrenOffset, char* bytes)
{
	std::cout << "New ";
	for(int i{0};i<4;i++)
	{
	 std::cout << bytes[i];
	}
	std::cout << " chunk" << '\n';
}

SizeChunk::SizeChunk(uint32_t childrenOffset, char* bytes)
	: Chunk{childrenOffset, bytes}
{}
} //namespace vxl
