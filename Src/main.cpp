#include "model.h"
#include "volume.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <numbers>
#include <raylib.h>
#include <raymath.h>
#include <string>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <random>
#endif //  defined ()

void UpdateLoop();

Camera cam;
vxl::Model* model;

int main()
{
#if defined(PLATFORM_WEB)
	std::string fileName;
	std::random_device rnd;
	std::mt19937 gen(rnd());
	{
		namespace fs = std::filesystem;
		std::vector<std::string> files;
		for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
		{
			files.push_back(
				entry.path().string().erase(0, strlen(RESOURCES_PATH)));
		}
		std::cout << files.size() << '\n';
		std::uniform_int_distribution<> random(0, files.size() - 1);
		fileName = files[random(gen)];
	}
#else
	std::cout << "Chose a file to load\n\n";
	{
		namespace fs = std::filesystem;
		for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
		{
			std::string name =
				entry.path().string().erase(0, strlen(RESOURCES_PATH));
			std::cout << '\t' << name << '\n';
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
#endif

	InitWindow(800, 800, NAME);

	model = new vxl::Model(RESOURCES_PATH + fileName);

	float dist =
		std::max(model->frames[0].bounds.x, model->frames[0].bounds.y) * 2.0f;

	{
		using namespace std::numbers;
		cam = *new Camera({.position = Vector3RotateByAxisAngle(
							   Vector3RotateByAxisAngle({dist, 0.0f, 0.0f},
														{0.0f, 0.0f, 1.0f},
														pi_v<float> / 4.0f),
							   {0.0f, 1.0f, 0.0f}, pi_v<float> / 4.0f),
						   .target = {0.0f, 0.0f, 0.0f},
						   .up = {0.0f, 1.0f, 0.0f},
						   .fovy = 45.0f,
						   .projection = 0});
	}

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateLoop, 0, 1);
#else
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		UpdateLoop();
	}
#endif

	delete model;
	CloseWindow();

	return 0;
}

void UpdateLoop()
{
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 mouseDelta{GetMouseDelta()};

		cam.position = Vector3RotateByAxisAngle(
			cam.position, {0.0f, 1.0f, 0.0f}, -mouseDelta.x * 0.005f);
		cam.position = Vector3RotateByAxisAngle(
			cam.position,
			Vector3CrossProduct(
				Vector3Subtract({0.0f, 0.0f, 0.0f}, cam.position),
				{0.0f, 1.0f, 0.0f}),
			-mouseDelta.y * 0.01f);
	}

	BeginDrawing();

	ClearBackground({100, 149, 237, 255});
	BeginMode3D(cam);

	vxl::DrawVolume(model);

	EndMode3D();
	DrawFPS(0, 0);
	EndDrawing();
}
