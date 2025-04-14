#include "model.h"
#include "chunk.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <string>
#include <windows.h>
#include <winnt.h>

#define FOREGROUND_WHITE 0xf
#define FOREGROUND_GREY 0x7
#define FOREGROUND_YELLOW 0xe

namespace vxl
{
using std::ios;

Model::Model(const std::string& filePath)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::streampos fSize;
	char* fileData;

	std::ifstream file(filePath.c_str(), ios::in | ios::binary | ios::ate);
	SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
	std::cout << '\n' << "Loading ";
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREY);
	std::cout << "\"" << filePath << "\"" << '\n';
	if (file.is_open())
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);

		//Load file into memory
		fSize = file.tellg();
		fileData = new char[fSize];
		file.seekg(0, ios::beg);
		file.read(fileData, fSize);
		file.close();

		std::copy(fileData, fileData + 4, this->ID.begin());
		std::memcpy(&this->version, &fileData[4], 4);

		std::cout << this->ID << "file found:" << '\n';
		std::cout << "Version: ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
		std::cout << this->version << '\n';

		SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
		ProcessChunks(&fileData[8]);

		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		std::cout << "Load Successful!" << '\n';

		delete[] fileData;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		std::cout << "Unable to open " << filePath << '\n';
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
}
void Model::ProcessChunks(char* bytes)
{
	char* endAddr = &bytes[*(bytes + 1) + *(bytes + 2) + 12];
	char* addr{bytes + 12};
	while (addr < endAddr)
	{
		uint32_t chunkID;
		uint32_t chunkContent;
		uint32_t chunkChildren;
		std::memcpy(&chunkID, addr, 4);
		std::memcpy(&chunkContent, addr + 4, 4);
		std::memcpy(&chunkChildren, addr + 8, 4);
		Chunk* chunk;
		switch (chunkID)
		{
		case 0x455A4953:
			chunk = new SizeChunk(chunkContent + 12, addr);
			break;
		case 0x495A5958:
			chunk = new XYZIChunk(chunkContent + 12, addr);
			break;
		case 0x4B434150:
			chunk = new PackChunk(chunkContent + 12, addr);
			break;
		default:
			chunk = new Chunk(chunkContent + 12, addr);
			break;
		}
		std::unique_ptr<Chunk> uChunk(chunk);
		this->chunks.push_back(std::move(uChunk));
		addr += (chunkContent + chunkChildren + 12);
		std::cout << "Chunk Processed" << '\n';
	}
	std::cout << "Done processing chunks!" << '\n';
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
