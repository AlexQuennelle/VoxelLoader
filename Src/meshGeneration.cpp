#include "meshGeneration.h"

#include <cstring>
#include <iostream>
#include <raylib.h>

namespace vxl
{

Mesh GenerateVoxelMesh(const vector<int16_t>&  /*volume*/, Vector3Int  /*bounds*/)
{
	Mesh mesh;
	vector<float> verts{
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	vector<float> uvs{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
					 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	vector<uint8_t> cols{
		255, 0,	  0,   255, 0,	 255, 0,   255, 0,	 0,	  255, 255, 255, 0,
		0,	 255, 0,   255, 0,	 255, 0,   0,	255, 255, 255, 0,	0,	 255,
		0,	 255, 0,   255, 0,	 0,	  255, 255, 255, 0,	  0,   255, 0,	 255,
		0,	 255, 0,   0,	255, 255, 255, 0,	0,	 255, 0,   255, 0,	 255,
		0,	 0,	  255, 255, 255, 0,	  0,   255, 0,	 255, 0,   255, 0,	 0,
		255, 255, 255, 0,	0,	 255, 0,   255, 0,	 255, 0,   0,	255, 255,
		255, 0,	  0,   255, 0,	 255, 0,   255, 0,	 0,	  255, 255,
	};
	vector<float> nors{0.0f,  0.0f,	 1.0f,	0.0f,  0.0f,  1.0f,	 0.0f,	0.0f,
					   1.0f,  0.0f,	 0.0f,	1.0f,  0.0f,  0.0f,	 -1.0f, 0.0f,
					   0.0f,  -1.0f, 0.0f,	0.0f,  -1.0f, 0.0f,	 0.0f,	-1.0f,
					   0.0f,  1.0f,	 0.0f,	0.0f,  1.0f,  0.0f,	 0.0f,	1.0f,
					   0.0f,  0.0f,	 1.0f,	0.0f,  0.0f,  -1.0f, 0.0f,	0.0f,
					   -1.0f, 0.0f,	 0.0f,	-1.0f, 0.0f,  0.0f,	 -1.0f, 0.0f,
					   1.0f,  0.0f,	 0.0f,	1.0f,  0.0f,  0.0f,	 1.0f,	0.0f,
					   0.0f,  1.0f,	 0.0f,	0.0f,  -1.0f, 0.0f,	 0.0f,	-1.0f,
					   0.0f,  0.0f,	 -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,	0.0f};

	mesh.vertices = (float*)std::malloc(verts.size() * sizeof(float));
	std::memcpy(mesh.vertices, verts.data(), verts.size() * sizeof(float));

	mesh.texcoords = (float*)std::malloc(uvs.size() * sizeof(float));
	std::memcpy(mesh.texcoords, uvs.data(), uvs.size() * sizeof(float));

	mesh.normals = (float*)std::malloc(nors.size() * sizeof(float));
	std::memcpy(mesh.normals, nors.data(), nors.size() * sizeof(float));

	mesh.colors = (uint8_t*)std::malloc(cols.size() * sizeof(uint8_t));
	std::memcpy(mesh.colors, cols.data(), cols.size() * sizeof(uint8_t));

	mesh.indices = (uint16_t*)std::malloc(36 * sizeof(uint16_t));
	int k__{0};
	for (int i = 0; i < 36; i += 6)
	{
		mesh.indices[i] = 4 * k__;
		mesh.indices[i + 1] = 4 * k__ + 1;
		mesh.indices[i + 2] = 4 * k__ + 2;
		mesh.indices[i + 3] = 4 * k__;
		mesh.indices[i + 4] = 4 * k__ + 2;
		mesh.indices[i + 5] = 4 * k__ + 3;

		k__++;
	}
	std::cout << '\n';
	for (uint32_t i{0}; i < 36; i += 3)
	{
		std::cout << mesh.indices[i] << ',';
		std::cout << mesh.indices[i + 1] << ',';
		std::cout << mesh.indices[i + 2] << '\n';
	}
	std::cout << '\n';

	mesh.vertexCount = 24;
	mesh.triangleCount = 12;

	return mesh;
};

} //namespace vxl
