#include "model.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <string>

namespace vxl
{

Model::Model(const std::string& filePath)
{
	std::streampos fSize;
	char* memblock;

	std::ifstream file(filePath.c_str(),
					   std::ios::in | std::ios::binary | std::ios::ate);
	std::cout << "Loading " << filePath << '\n';
	if (file.is_open())
	{
		fSize = file.tellg();
		memblock = new char[fSize];
		file.seekg(0, std::ios::beg);
		file.read(memblock, fSize);
		this->header[0] = memblock[0];

		std::cout << memblock[0] << '\n';

		file.close();
	}
	else
	{
		std::cout << "Unable to open " << filePath << '\n';
	}
}

} //namespace vxl
