#include "vxlMesh.h"

#include <raylib.h>
#include <raymath.h>

namespace vxl
{

VXLMesh::VXLMesh(const std::string& vert, const std::string& frag)
{
	this->shader = LoadShader(vert.c_str(), frag.c_str());
	this->mat = LoadMaterialDefault();
	this->mat.shader = this->shader;
}

void VXLMesh::Draw()
{
	for (auto mesh : this->meshes)
	{
		DrawMesh(mesh, this->mat, MatrixIdentity());
	}
}

void VXLMesh::AddMesh(Mesh mesh)
{
	this->meshes.push_back(mesh);
	//UploadMesh(&this->meshes[this->meshes.size() - 1], false);
}

} //namespace vxl
