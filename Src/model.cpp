#include "model.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>
#include <windows.h>
#include <winnt.h>

namespace vxl
{
using std::ios;

Model::Model(const std::string& filePath)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::streampos fSize;
	char* memblock;

	std::ifstream file(filePath.c_str(), ios::in | ios::binary | ios::ate);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN |
										  FOREGROUND_INTENSITY);
	std::cout << '\n' << "Loading " << filePath << '\n';
	if (file.is_open())
	{
		fSize = file.tellg();
		memblock = new char[fSize];
		file.seekg(0, ios::beg);
		file.read(memblock, fSize);

		std::copy(memblock, memblock + 4, this->header.begin());
		std::copy(memblock + 5, memblock + 8, &this->version);

		// TODO: Fix endianness on version number

		std::cout << this->header << '\n';
		std::cout << this->version << '\n';

		file.close();

		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		std::cout << "Load Successful!" << '\n';
		delete (memblock);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		std::cout << "Unable to open " << filePath << '\n';
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN |
										  FOREGROUND_BLUE |
										  FOREGROUND_INTENSITY);
}

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& data)
{
	for (uint64_t i{0}; i < data.size(); i++)
	{
		os << data[i];
	}
	return os;
}

} //namespace vxl
