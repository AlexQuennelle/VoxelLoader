#pragma once

#include "vxlMesh.h"
#include "vxlModel.h"

#include <cstdint>
#include <raylib.h>
#include <vector>

namespace vxl
{

using std::vector;

vxlMesh GenerateVoxelMesh(const vector<int16_t>& volume, Vector3Int bounds,
						  const array<uint32_t, 256>& palette);

void PopulateMesh(Mesh& mesh, vector<float>& verts, vector<uint8_t>& cols,
				  vector<float>& nors, int32_t triCount);

int32_t AddVoxel(Vector3 offset, vector<float>& verts, vector<uint8_t>& cols,
				 vector<float>& nors, int32_t triCount, uint8_t mask,
				 uint32_t* col);

} //namespace vxl
