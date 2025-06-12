#pragma once

#include "model.h"

#include <cstdint>
#include <raylib.h>
#include <vector>

namespace vxl
{

using std::vector;

Mesh GenerateVoxelMesh(const vector<int16_t>& volume, Vector3Int bounds);

} //namespace vxl
