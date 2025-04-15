#include "model.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <winnt.h>
#include <winscard.h>

namespace vxl
{
using std::ios;

#define FOREGROUND_WHITE 0xf
#define FOREGROUND_GREY 0x7
#define FOREGROUND_YELLOW 0xe

#define BOUNDINGBOX 0x455A4953
#define VOXELDATA 0x495A5958
#define ANIMDATA 0x4B434150

std::ostream& operator<<(std::ostream& os, Vector3Int vec)
{
	os << '(' << static_cast<uint32_t>(vec.x) << ", "
	   << static_cast<uint32_t>(vec.y) << ", " << static_cast<uint32_t>(vec.z)
	   << ')';
	return os;
}
std::ostream& operator<<(std::ostream& os, Vector4Int vec)
{
	os << '(' << static_cast<uint32_t>(vec.x) << ", "
	   << static_cast<uint32_t>(vec.y) << ", " << static_cast<uint32_t>(vec.z)
	   << ", " << static_cast<uint32_t>(vec.i) << ')';
	return os;
}

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
		ProcessChunks(fileData + 8);

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
	uint32_t fileContent;
	uint32_t fileChildren;
	std::memcpy(&fileContent, bytes + 4, 4);
	std::memcpy(&fileChildren, bytes + 8, 4);
	char* endAddr = bytes + (fileContent + fileChildren + 12);
	char* addr{bytes + 12};
	//std::cout << std::hex << endAddr - bytes + 12;
	while (addr < endAddr)
	{
		//std::cout << std::hex << addr - bytes + 8 << '\n';
		uint32_t chunkID;
		uint32_t chunkContent;
		uint32_t chunkChildren;
		std::memcpy(&chunkID, addr, 4);
		std::memcpy(&chunkContent, addr + 4, 4);
		std::memcpy(&chunkChildren, addr + 8, 4);
		char* nextAddr = addr + (chunkContent + chunkChildren + 12);

		switch (chunkID)
		{
		case ANIMDATA:
			// TODO: Fix animations
			uint32_t frameCount;
			std::memcpy(&frameCount, addr + 12, 4);
			this->frameCount = frameCount;
			break;
		case BOUNDINGBOX:
			AddFrame(addr, nextAddr);
			uint32_t nextContent;
			uint32_t nextChildren;
			std::memcpy(&nextContent, nextAddr + 4, 4);
			std::memcpy(&nextChildren, nextAddr + 8, 4);
			addr += (nextContent + nextChildren + 12);
			break;
		case VOXELDATA:
			std::cout << "Skip" << '\n';
			break;
		default:
			break;
		}
		addr += (chunkContent + chunkChildren + 12);
	}
	std::cout << "Done processing chunks!" << '\n';
}
void Model::AddFrame(char* boundData, char* voxelData)
{
	std::cout << "New frame" << '\n';
	Vector3Int bounds;
	std::memcpy(&bounds, boundData + 12, 12);
	uint32_t voxelCount;
	std::memcpy(&voxelCount, voxelData + 12, 4);
	AnimationFrame frame{};
	frame.bounds = bounds;
	frame.voxels.reserve(voxelCount);
	for (uint32_t i{0}; i < voxelCount; i++)
	{
		frame.voxels.push_back(
			{.x = static_cast<uint8_t>(*(voxelData + 16 + (i * 4))),
			 .y = static_cast<uint8_t>(*(voxelData + 18 + (i * 4))),
			 .z = static_cast<uint8_t>(*(voxelData + 17 + (i * 4))),
			 .i = static_cast<uint8_t>(*(voxelData + 19 + (i * 4)))});
	}
	this->frames.push_back(std::move(frame));
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
