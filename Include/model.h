#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

namespace vxl
{
using std::array;

class Model
{
	public:
	Model(const std::string& filePath);

	array<char, 4> header;
	uint32_t version;
};

template<std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& data);

} //namespace vxl
