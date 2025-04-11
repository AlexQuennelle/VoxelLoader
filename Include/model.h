#pragma once

#include "chunk.h"
#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace vxl
{
using std::array;

class Model
{
	public:
	Model(const std::string& filePath);

	array<char, 4> ID;
	uint32_t version;
	std::vector<std::unique_ptr<Chunk>> chunks;

	private:
	void ProcessChunks(char* bytes);
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& data);

} //namespace vxl
