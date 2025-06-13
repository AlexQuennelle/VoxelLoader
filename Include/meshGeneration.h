#pragma once

#include "model.h"

#include <cstdint>
#include <raylib.h>
#include <vector>

namespace vxl
{

using std::vector;

Mesh GenerateVoxelMesh(const vector<int16_t>& volume, Vector3Int bounds);

int32_t AddVoxel(Vector3 offset, vector<float>& verts, vector<uint8_t>& cols,
				 vector<float>& nors, vector<uint16_t>& indices,
				 int32_t triCount);

} //namespace vxl
