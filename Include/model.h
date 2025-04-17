#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace vxl
{
using std::array;

struct Vector3Int
{
	public:
	int32_t x, z, y;
};
struct Vector4Int
{
	uint8_t x, y, z, i;
};

struct AnimationFrame
{
	public:
	Vector3Int bounds;
	std::vector<Vector4Int> voxels;
};

class Model
{
	public:
	Model(const std::string& filePath);

	array<char, 4> ID;
	uint32_t version;

	std::vector<AnimationFrame> frames;
	uint32_t frameCount{1};
	uint32_t curFrame{0};
	std::array<uint32_t, 256> palette{};

	private:
	void ProcessChunks(char* bytes);
	void AddFrame(char* boundData, char* voxelData);
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& data);

} //namespace vxl
