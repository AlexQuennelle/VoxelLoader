#include "meshGeneration.h"
#include "vxlMesh.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <raylib.h>
#include <vector>

namespace vxl
{

VXLMesh GenerateVoxelMesh(const vector<int16_t>& volume, Vector3Int bounds)
{
	VXLMesh vxlMesh{RESOURCES_PATH "shaders/litShader.vert",
					RESOURCES_PATH "shaders/litShader.frag"};
	Vector3 offset{};
	int32_t triangleCount{0};
	vector<float> verts{};
	vector<uint8_t> cols{};
	vector<float> nors{};
	vector<uint16_t> indices{};
	for (uint32_t i{0}; i < volume.size(); i++)
	{
		int32_t x = i / (bounds.y * bounds.z);
		int32_t y = (i / bounds.z) % bounds.y;
		int32_t z = i % bounds.z;
		if (volume[i] != -1)
		{
			uint8_t mask{0};
			uint32_t up = (x * bounds.y * bounds.z) + ((y + 1) * bounds.z) + z;
			uint32_t down =
				(x * bounds.y * bounds.z) + ((y - 1) * bounds.z) + z;
			uint32_t left =
				((x + 1) * bounds.y * bounds.z) + (y * bounds.z) + z;
			uint32_t right =
				((x - 1) * bounds.y * bounds.z) + (y * bounds.z) + z;
			uint32_t front =
				(x * bounds.y * bounds.z) + (y * bounds.z) + (z - 1);
			uint32_t back =
				(x * bounds.y * bounds.z) + (y * bounds.z) + (z + 1);
			//std::cout << i << '\n';
			offset = {
				.x = static_cast<float>(x) - (static_cast<float>(bounds.x) / 2),
				.y = static_cast<float>(y) - (static_cast<float>(bounds.y) / 2),
				.z =
					static_cast<float>(z) - (static_cast<float>(bounds.z) / 2)};
			mask |= (x == bounds.x - 1 || volume[left] == -1) ? 0b00000001 : 0;
			mask |= (x == 0 || volume[right] == -1) ? 0b00000010 : 0;
			mask |= (y == bounds.y - 1 || volume[up] == -1) ? 0b00000100 : 0;
			mask |= (y == 0 || volume[down] == -1) ? 0b00001000 : 0;
			mask |= (z == bounds.z - 1 || volume[back] == -1) ? 0b00010000 : 0;
			mask |= (z == 0 || volume[front] == -1) ? 0b00100000 : 0;
			triangleCount = AddVoxel(offset, verts, cols, nors, indices,
									 triangleCount, mask);
			//std::cout << "Triangles: " << triangleCount << '\n';
			if (triangleCount * 3 > std::numeric_limits<uint16_t>::max() - 12)
			{
				vxlMesh.AddMesh(PopulateMesh(verts, cols, nors, triangleCount));
				triangleCount = 0;
				verts.clear();
				cols.clear();
				nors.clear();
			}
		}
	}
	if (triangleCount != 0)
	{
		vxlMesh.AddMesh(PopulateMesh(verts, cols, nors, triangleCount));
	}
	std::cout << "meshes: " << vxlMesh.meshes.size() << '\n';
	return vxlMesh;
};

Mesh PopulateMesh(const vector<float>& verts, const vector<uint8_t>& cols,
				  const vector<float>& nors, const int32_t triCount)
{
	Mesh mesh;
	std::cout << "Triangles: " << triCount << '/'
			  << std::numeric_limits<uint16_t>::max() / 3 << '\n';
	std::cout << "Vertices: " << verts.size() << '\n';
	// for (uint32_t i{0}; i < verts.size(); i += 3)
	// {
	// 	verts[i] -= static_cast<float>(bounds.x) / 2;
	// 	verts[i + 1] -= static_cast<float>(bounds.y) / 2;
	// 	verts[i + 2] -= static_cast<float>(bounds.z) / 2;
	// }

	mesh.vertices = (float*)std::malloc(verts.size() * sizeof(float));
	std::memcpy(mesh.vertices, verts.data(), verts.size() * sizeof(float));

	mesh.normals = (float*)std::malloc(nors.size() * sizeof(float));
	std::memcpy(mesh.normals, nors.data(), nors.size() * sizeof(float));

	mesh.colors = (uint8_t*)std::malloc(cols.size() * sizeof(uint8_t));
	std::memcpy(mesh.colors, cols.data(), cols.size() * sizeof(uint8_t));

	mesh.indices = (uint16_t*)std::malloc(triCount * 3 * sizeof(uint16_t));
	int k{0};
	for (int i = 0; i < triCount * 3; i += 6)
	{
		mesh.indices[i] = 4 * k;
		mesh.indices[i + 1] = 4 * k + 1;
		mesh.indices[i + 2] = 4 * k + 2;
		mesh.indices[i + 3] = 4 * k;
		mesh.indices[i + 4] = 4 * k + 2;
		mesh.indices[i + 5] = 4 * k + 3;

		k++;
	}

	mesh.vertexCount = verts.size() / 3;
	mesh.triangleCount = triCount;

	//assert(mesh.indices == NULL);

	return mesh;
}

int32_t AddVoxel(Vector3 offset, vector<float>& verts, vector<uint8_t>& cols,
				 vector<float>& nors, vector<uint16_t>& indices,
				 int32_t triCount, uint8_t mask)
{
	int32_t newTriCount{triCount};
	vector<float> newVerts{};

	//Back face
	if ((mask & 0b00010000) != 0)
	{
		const vector<float> face1Verts{0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
									   //1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
									   1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
		const vector<uint8_t> face1Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face1Nors{0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
									  //0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
									  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
		newVerts.insert(newVerts.end(), face1Verts.begin(), face1Verts.end());
		cols.insert(cols.end(), face1Cols.begin(), face1Cols.end());
		nors.insert(nors.end(), face1Nors.begin(), face1Nors.end());
		newTriCount += 2;
	}

	//Front face
	if ((mask & 0b00100000) != 0)
	{
		const vector<float> face2Verts{0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									   //1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
									   1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f};
		const vector<uint8_t> face2Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face2Nors{0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
									  //0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
									  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f};
		newVerts.insert(newVerts.end(), face2Verts.begin(), face2Verts.end());
		cols.insert(cols.end(), face2Cols.begin(), face2Cols.end());
		nors.insert(nors.end(), face2Nors.begin(), face2Nors.end());
		newTriCount += 2;
	}

	//Top face
	if ((mask & 0b00000100) != 0)
	{
		const vector<float> face3Verts{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
									   //1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
									   1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};
		const vector<uint8_t> face3Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face3Nors{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									  //0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
		newVerts.insert(newVerts.end(), face3Verts.begin(), face3Verts.end());
		cols.insert(cols.end(), face3Cols.begin(), face3Cols.end());
		nors.insert(nors.end(), face3Nors.begin(), face3Nors.end());
		newTriCount += 2;
	}

	//Bottom face
	if ((mask & 0b00001000) != 0)
	{
		const vector<float> face4Verts{0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									   //1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
									   1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
		const vector<uint8_t> face4Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face4Nors{0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
									  //0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
									  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f};
		newVerts.insert(newVerts.end(), face4Verts.begin(), face4Verts.end());
		cols.insert(cols.end(), face4Cols.begin(), face4Cols.end());
		nors.insert(nors.end(), face4Nors.begin(), face4Nors.end());
		newTriCount += 2;
	}

	//Left face
	if ((mask & 0b00000001) != 0)
	{
		const vector<float> face5Verts{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
									   //1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
									   1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f};
		const vector<uint8_t> face5Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face5Nors{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									  //1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
		newVerts.insert(newVerts.end(), face5Verts.begin(), face5Verts.end());
		cols.insert(cols.end(), face5Cols.begin(), face5Cols.end());
		nors.insert(nors.end(), face5Nors.begin(), face5Nors.end());
		newTriCount += 2;
	}

	//face 6
	if ((mask & 0b00000010) != 0)
	{
		const vector<float> face6Verts{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
									   //0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
									   0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
		const vector<uint8_t> face6Cols{
			255, 0, 0, 255, 0, 255, 0, 255,
			//0,	 0,	  255, 255, 255, 0,	  0,   255,
			0, 255, 0, 255, 0, 0, 255, 255};
		const vector<float> face6Nors{-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
									  //-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
									  -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
		newVerts.insert(newVerts.end(), face6Verts.begin(), face6Verts.end());
		cols.insert(cols.end(), face6Cols.begin(), face6Cols.end());
		nors.insert(nors.end(), face6Nors.begin(), face6Nors.end());
		newTriCount += 2;
	}

	for (uint32_t i{0}; i < newVerts.size(); i += 3)
	{
		newVerts[i] += offset.x;
		newVerts[i + 1] += offset.y;
		newVerts[i + 2] += offset.z;
	}
	verts.insert(verts.end(), newVerts.begin(), newVerts.end());
	return newTriCount;
}

} //namespace vxl
