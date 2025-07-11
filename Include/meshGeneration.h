#pragma once

#include "model.h"
#include "vxlMesh.h"

#include <cstdint>
#include <raylib.h>
#include <vector>

namespace vxl
{

using std::vector;

vxlMesh GenerateVoxelMesh(const vector<int16_t>& volume, Vector3Int bounds);

void PopulateMesh(Mesh& mesh, vector<float>& verts, vector<uint8_t>& cols,
				  vector<float>& nors, int32_t triCount);

int32_t AddVoxel(Vector3 offset, vector<float>& verts, vector<uint8_t>& cols,
				 vector<float>& nors, int32_t triCount, uint8_t mask);

} //namespace vxl
