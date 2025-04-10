#pragma once

#include <array>
#include <string>

namespace vxl
{

class Model
{
	public:
	Model(const std::string& filePath);

	std::array<char, 4> header;
};

} //namespace vxl
