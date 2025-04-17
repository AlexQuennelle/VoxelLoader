#include "volume.h"
#include "model.h"

#include <array>
#include <cstdint>
#include <raylib.h>
#include <vector>

namespace vxl
{

uint8_t renderedFrames{0};

#define BOUNDINGBOX 0x455A4953
#define VOXELDATA 0x495A5958
#define ANIMDATA 0x4B434150

void DrawVolume(Model* model)
{
	Vector3Int bounds = model->frames[model->curFrame].bounds;
	for (auto voxel : model->frames[model->curFrame].voxels)
	{
		std::array<uint8_t, 4> col;
		std::memcpy(&col, &model->palette[voxel.i], 4);
		DrawCube({static_cast<float>(voxel.x - (bounds.x / 2)),
				  static_cast<float>(voxel.y - (bounds.y / 2)),
				  static_cast<float>(voxel.z - (bounds.z / 2))},
				 1.0f, 1.0f, 1.0f, {col[0], col[1], col[2], col[3]});
	}
	if (renderedFrames < 5)
	{
		renderedFrames++;
	}
	else
	{
		renderedFrames = 0;
		model->curFrame = (model->curFrame + 1) % model->frameCount;
	}
}

} //namespace vxl
