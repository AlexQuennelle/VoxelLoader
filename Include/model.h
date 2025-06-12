#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>
#include <raylib.h>

namespace vxl
{
using std::array;
using std::vector;

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
	vector<Vector4Int> voxels;
	Mesh mesh;
};

class vxlModel
{
	public:
	vxlModel(const std::string& filePath);

	array<char, 4> ID;
	uint32_t version;

	//vector<AnimationFrame> frames;
	vector<Model> frames;
	vector<Mesh> meshes;
	uint32_t frameCount{1};
	uint32_t curFrame{0};
	array<uint32_t, 256> palette{};

	private:
	void ProcessChunks(char* bytes);
	void AddFrame(char* boundData, char* voxelData);
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const array<char, N>& data);

} //namespace vxl
