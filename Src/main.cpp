#include "model.h"
#include "utils.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <numbers>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <string>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <random>
#endif //  defined ()

void Update();
void vxlDrawModel(Model model);
void vxlDrawMesh(Mesh mesh, Material material, Matrix transform);

Camera cam;
vxl::vxlModel* vxlmodel;
Vector2 camSpeed;
Shader shader;
uint8_t renderedFrames{0};

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
#if defined(PLATFORM_WEB)
	std::string fileName;
	std::random_device rnd;
	std::mt19937 gen(rnd());
	{
		namespace fs = std::filesystem;
		std::vector<std::string> files;
		for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
		{
			std::string name =
				entry.path().string().erase(0, strlen(RESOURCES_PATH));
			if (name.ends_with(".vox"))
			{
				files.push_back(
					entry.path().string().erase(0, strlen(RESOURCES_PATH)));
			}
		}
		std::cout << files.size() << '\n';
		std::uniform_int_distribution<> random(0, files.size() - 1);
		fileName = files[random(gen)];
	}

	InitWindow(400, 400, NAME);
	shader = LoadShader(RESOURCES_PATH "shaders/web/litShader_web.vert",
						RESOURCES_PATH "shaders/web/litShader_web.frag");
#else
	std::cout << "Chose a file to load\n\n";
	{
		namespace fs = std::filesystem;
		for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
		{
			std::string name =
				entry.path().string().erase(0, strlen(RESOURCES_PATH));
			if (name.ends_with(".vox"))
			{
				std::cout << '\t' << name << '\n';
			}
		}
	}
	std::string fileName;
	std::cout << '\n';
	std::cout << "Enter file name: ";
	std::cin >> fileName;

	// TODO: make this fancier later
	if (!std::filesystem::exists(RESOURCES_PATH + fileName))
	{
		std::cout << RESOURCES_PATH << fileName << " does not exist\n";
		return -1;
	}
	if (!fileName.ends_with(".vox"))
	{
		std::cout << RESOURCES_PATH << fileName << " is not a vox file\n";
		return -1;
	}

	InitWindow(800, 800, NAME);
	shader = LoadShader(RESOURCES_PATH "shaders/litShader.vert",
						RESOURCES_PATH "shaders/litShader.frag");
#endif
	vxlmodel = new vxl::vxlModel(RESOURCES_PATH + fileName);

	float vFOV = 45.0f;
	float aspectRatio = static_cast<float>(GetScreenWidth()) /
						static_cast<float>(GetScreenHeight());
	float fov =
		std::min(vFOV, RAD2DEG * (2 * std::atan(std::tan((DEG2RAD * vFOV) / 2) *
												aspectRatio)));
	auto mBounds =
		Vector3(vxlmodel->bounds.x, vxlmodel->bounds.y, vxlmodel->bounds.z);
	float rad =
		sqrt((pow(mBounds.x, 2) + pow(mBounds.y, 2)) + pow(mBounds.z, 2)) / 2;
	float dist = rad * (std::sin(DEG2RAD * 90) / std::sin(DEG2RAD * (fov / 2)));
	camSpeed = {.x = 0.36f - std::pow(dist * (1.0f / 25000.0f), 1.0f / 3.5f),
				.y = 0.97f - std::pow(dist * 1.0f / 500.0f, 1.0f / 5.0f)};

	{
		using namespace std::numbers;
		cam = *new Camera({.position = Vector3RotateByAxisAngle(
							   Vector3RotateByAxisAngle({dist, 0.0f, 0.0f},
														{0.0f, 0.0f, 1.0f},
														pi_v<float> / 12.0f),
							   {0.0f, 1.0f, 0.0f}, pi_v<float> / 4.0f),
						   .target = {0.0f, 0.0f, 0.0f},
						   .up = {0.0f, 1.0f, 0.0f},
						   .fovy = vFOV,
						   .projection = 0});
	}

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(Update, 0, 1);
#else
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		Update();
	}
#endif

	delete vxlmodel;
	CloseWindow();

	return 0;
}

void Update()
{
	float dt = GetFrameTime();
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 mouseDelta{GetMouseDelta()};

		cam.position = Vector3RotateByAxisAngle(
			cam.position, {0.0f, 1.0f, 0.0f}, -mouseDelta.x * camSpeed.x * dt);
		float dAngle = -mouseDelta.y * camSpeed.y * dt;
		float currentAngle =
			Vector3Angle(Vector3Normalize(cam.position), {0.0f, 1.0f, 0.0f});
		if (currentAngle + dAngle > 175.0f * DEG2RAD)
		{
			dAngle -= (currentAngle + dAngle) - (175.0f * DEG2RAD);
		}
		else if (currentAngle + dAngle < 5.0f * DEG2RAD)
		{
			dAngle += (5.0f * DEG2RAD) - (currentAngle + dAngle);
		}
		cam.position = Vector3RotateByAxisAngle(
			cam.position,
			Vector3CrossProduct(
				Vector3Subtract({0.0f, 0.0f, 0.0f}, cam.position),
				{0.0f, 1.0f, 0.0f}),
			dAngle);
	}

	BeginDrawing();

	ClearBackground({100, 149, 237, 255});
	BeginMode3D(cam);

	Model model = LoadModelFromMesh(vxlmodel->meshes[vxlmodel->curFrame]);
	model.materials->shader = shader;
	//DrawModel(model, {0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
	vxlDrawModel(model);
	if (renderedFrames < 5)
	{
		renderedFrames++;
	}
	else
	{
		renderedFrames = 0;
		vxlmodel->curFrame = (vxlmodel->curFrame + 1) % vxlmodel->frameCount;
	}

	EndMode3D();
	DrawFPS(0, 0);
	EndDrawing();
}
void vxlDrawModel(Model model)
{
	const Vector3 scale{1.0f, 1.0f, 1.0f};
	const Vector3 rotationAxis{0.0f, 1.0f, 0.0f};
	const Vector3 position{0.0f, 0.0f, 0.0f};
	const float rotationAngle{0.0f};
	const Color tint = WHITE;
	// Calculate transformation matrix from function parameters
	// Get transform matrix (rotation -> scale -> translation)
	Matrix matScale = MatrixScale(scale.x, scale.y, scale.z);
	Matrix matRotation = MatrixRotate(rotationAxis, rotationAngle * DEG2RAD);
	Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);

	Matrix matTransform =
		MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

	// Combine model transformation matrix (model.transform) with matrix
	// generated by function parameters (matTransform)
	model.transform = MatrixMultiply(model.transform, matTransform);

	for (int i = 0; i < model.meshCount; i++)
	{
		Color color = model.materials[model.meshMaterial[i]]
						  .maps[MATERIAL_MAP_DIFFUSE]
						  .color;

		Color colorTint = WHITE;
		colorTint.r = (unsigned char)(((int)color.r * (int)tint.r) / 255);
		colorTint.g = (unsigned char)(((int)color.g * (int)tint.g) / 255);
		colorTint.b = (unsigned char)(((int)color.b * (int)tint.b) / 255);
		colorTint.a = (unsigned char)(((int)color.a * (int)tint.a) / 255);

		model.materials[model.meshMaterial[i]]
			.maps[MATERIAL_MAP_DIFFUSE]
			.color = colorTint;
		vxlDrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]],
					model.transform);
		model.materials[model.meshMaterial[i]]
			.maps[MATERIAL_MAP_DIFFUSE]
			.color = color;
	}
}
void vxlDrawMesh(Mesh mesh, Material material, Matrix transform)
{
	// Bind shader program
	rlEnableShader(material.shader.id);

	// Send required data to shader (matrices, values)
	//-----------------------------------------------------
	// Upload to shader material.colDiffuse
	if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1)
	{
		float values[4] = {
			(float)material.maps[MATERIAL_MAP_DIFFUSE].color.r / 255.0f,
			(float)material.maps[MATERIAL_MAP_DIFFUSE].color.g / 255.0f,
			(float)material.maps[MATERIAL_MAP_DIFFUSE].color.b / 255.0f,
			(float)material.maps[MATERIAL_MAP_DIFFUSE].color.a / 255.0f};

		rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values,
					 SHADER_UNIFORM_VEC4, 1);
	}

	// Upload to shader material.colSpecular (if location available)
	if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
	{
		float values[4] = {
			(float)material.maps[MATERIAL_MAP_SPECULAR].color.r / 255.0f,
			(float)material.maps[MATERIAL_MAP_SPECULAR].color.g / 255.0f,
			(float)material.maps[MATERIAL_MAP_SPECULAR].color.b / 255.0f,
			(float)material.maps[MATERIAL_MAP_SPECULAR].color.a / 255.0f};

		rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values,
					 SHADER_UNIFORM_VEC4, 1);
	}

	// Get a copy of current matrices to work with,
	// just in case stereo render is required, and we need to modify them
	// NOTE: At this point the modelview matrix just contains the view matrix
	// (camera) That's because BeginMode3D() sets it and there is no
	// model-drawing function that modifies it, all use rlPushMatrix() and
	// rlPopMatrix()
	Matrix matModel = MatrixIdentity();
	Matrix matView = rlGetMatrixModelview();
	Matrix matModelView = MatrixIdentity();
	Matrix matProjection = rlGetMatrixProjection();

	// Upload view and projection matrices (if locations available)
	if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1)
		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW],
						   matView);
	if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1)
		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION],
						   matProjection);

	// Accumulate several model transformations:
	//    transform: model transformation provided (includes DrawModel() params
	//    combined with model.transform) rlGetMatrixTransform(): rlgl internal
	//    transform matrix due to push/pop matrix stack
	matModel = MatrixMultiply(transform, rlGetMatrixTransform());

	// Model transformation matrix is sent to shader uniform location:
	// SHADER_LOC_MATRIX_MODEL
	if (material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1)
		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MODEL],
						   matModel);

	// Get model-view matrix
	matModelView = MatrixMultiply(matModel, matView);

	// Upload model normal matrix (if locations available)
	if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1)
		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL],
						   MatrixTranspose(MatrixInvert(matModel)));

	//-----------------------------------------------------

	// Bind active texture maps (if available)
	//for (int i = 0; i < 12; i++)
	//{
	//	if (material.maps[i].texture.id > 0)
	//	{
	//		// Select current shader texture slot
	//		rlActiveTextureSlot(i);

	//		// Enable texture for active slot
	//		if ((i == MATERIAL_MAP_IRRADIANCE) ||
	//			(i == MATERIAL_MAP_PREFILTER) || (i == MATERIAL_MAP_CUBEMAP))
	//			rlEnableTextureCubemap(material.maps[i].texture.id);
	//		else
	//			rlEnableTexture(material.maps[i].texture.id);

	//		rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i,
	//					 SHADER_UNIFORM_INT, 1);
	//	}
	//}

	// Try binding vertex array objects (VAO) or use VBOs if not possible
	// WARNING: UploadMesh() enables all vertex attributes available in mesh and
	// sets default attribute values for shader expected vertex attributes that
	// are not provided by the mesh (i.e. colors) This could be a dangerous
	// approach because different meshes with different shaders can
	// enable/disable some attributes
	if (!rlEnableVertexArray(mesh.vaoId))
	{
		// Bind mesh VBO data: vertex position (shader-location = 0)
		rlEnableVertexBuffer(
			mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION]);
		rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION],
							 3, RL_FLOAT, 0, 0, 0);
		rlEnableVertexAttribute(
			material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

		// Bind mesh VBO data: vertex texcoords (shader-location = 1)
		rlEnableVertexBuffer(
			mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD]);
		rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01],
							 2, RL_FLOAT, 0, 0, 0);
		rlEnableVertexAttribute(
			material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

		if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
		{
			// Bind mesh VBO data: vertex normals (shader-location = 2)
			rlEnableVertexBuffer(
				mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL]);
			rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL],
								 3, RL_FLOAT, 0, 0, 0);
			rlEnableVertexAttribute(
				material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
		}

		// Bind mesh VBO data: vertex colors (shader-location = 3, if available)
		if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
		{
			if (mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR] != 0)
			{
				rlEnableVertexBuffer(
					mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR]);
				rlSetVertexAttribute(
					material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4,
					RL_UNSIGNED_BYTE, 1, 0, 0);
				rlEnableVertexAttribute(
					material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
			}
			else
			{
				// Set default value for defined vertex attribute in shader but
				// not provided by mesh WARNING: It could result in GPU
				// undefined behaviour
				float value[4] = {1.0f, 1.0f, 1.0f, 1.0f};
				rlSetVertexAttributeDefault(
					material.shader.locs[SHADER_LOC_VERTEX_COLOR], value,
					SHADER_ATTRIB_VEC4, 4);
				rlDisableVertexAttribute(
					material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
			}
		}

		// Bind mesh VBO data: vertex tangents (shader-location = 4, if
		// available)
		if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
		{
			rlEnableVertexBuffer(
				mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT]);
			rlSetVertexAttribute(
				material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0,
				0, 0);
			rlEnableVertexAttribute(
				material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
		}

		// Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if
		// available)
		if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
		{
			rlEnableVertexBuffer(
				mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2]);
			rlSetVertexAttribute(
				material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT,
				0, 0, 0);
			rlEnableVertexAttribute(
				material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
		}

		if (mesh.indices != NULL)
		{
			vxl::SetTextColor(vxl::ERROR);
			std::cout << "Buzzer\n";
			vxl::SetTextColor(WHITE);
			rlEnableVertexBufferElement(
				mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES]);
		}
	}

	int eyeCount = 1;
	if (rlIsStereoRenderEnabled())
		eyeCount = 2;

	for (int eye = 0; eye < eyeCount; eye++)
	{
		// Calculate model-view-projection matrix (MVP)
		Matrix matModelViewProjection = MatrixIdentity();
		if (eyeCount == 1)
			matModelViewProjection =
				MatrixMultiply(matModelView, matProjection);
		else
		{
			// Setup current eye viewport (half screen width)
			rlViewport(eye * rlGetFramebufferWidth() / 2, 0,
					   rlGetFramebufferWidth() / 2, rlGetFramebufferHeight());
			matModelViewProjection = MatrixMultiply(
				MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)),
				rlGetMatrixProjectionStereo(eye));
		}

		// Send combined model-view-projection matrix to shader
		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP],
						   matModelViewProjection);

		// Draw mesh
		if (mesh.indices != NULL)
		{
			rlDrawVertexArrayElements(0, mesh.triangleCount * 3, 0);
		}
		else
		{
			rlDrawVertexArray(0, mesh.vertexCount);
		}
	}

	// Unbind all bound texture maps
	for (int i = 0; i < 12; i++)
	{
		if (material.maps[i].texture.id > 0)
		{
			// Select current shader texture slot
			rlActiveTextureSlot(i);

			// Disable texture for active slot
			if ((i == MATERIAL_MAP_IRRADIANCE) ||
				(i == MATERIAL_MAP_PREFILTER) || (i == MATERIAL_MAP_CUBEMAP))
				rlDisableTextureCubemap();
			else
				rlDisableTexture();
		}
	}

	// Disable all possible vertex array objects (or VBOs)
	rlDisableVertexArray();
	rlDisableVertexBuffer();
	rlDisableVertexBufferElement();

	// Disable shader program
	rlDisableShader();

	// Restore rlgl internal modelview and projection matrices
	rlSetMatrixModelview(matView);
	rlSetMatrixProjection(matProjection);
}
