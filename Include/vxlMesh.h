#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace vxl
{

class VXLMesh
{
	public:
	VXLMesh(const std::string& vert, const std::string& frag);
	void Draw();
	void AddMesh(Mesh mesh);
	std::vector<Mesh> meshes;

	private:
	Shader shader;
	Material mat;
};

} //namespace vxl
