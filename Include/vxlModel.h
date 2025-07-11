#pragma once

#include "vxlMesh.h"
#include <array>
#include <cstdint>
#include <ostream>
#include <raylib.h>
#include <string>
#include <vector>

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

class vxlModel
{
	public:
	vxlModel(const std::string& filePath);

	void Draw();

	Vector3Int bounds;
	array<char, 4> ID;
	uint32_t version;

	private:
	void ProcessChunks(char* bytes);
	void AddFrame(char* boundData, char* voxelData);

	uint32_t frameCount{1};
	uint32_t curFrame{0};
	Material mat;
	Shader shader;
	vector<vxlMesh> meshes;
	array<uint32_t, 256> palette{};
	uint8_t renderedFrames{0};
};

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const array<char, N>& data);

} //namespace vxl
