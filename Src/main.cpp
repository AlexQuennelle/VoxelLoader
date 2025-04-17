#include "model.h"
#include "volume.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>

int main()
{
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
		return -1;
	}

	InitWindow(800, 800, NAME);

	vxl::Model model(RESOURCES_PATH + fileName);

	float dist =
		std::max(model.frames[0].bounds.x, model.frames[0].bounds.y) * 2.0f;

	Camera cam{Vector3RotateByAxisAngle(
				   Vector3RotateByAxisAngle({dist, 0.0f, 0.0f},
											{0.0f, 0.0f, 1.0f}, PI / 4.0f),
				   {0.0f, 1.0f, 0.0f}, PI / 4.0f),
			   {0.0f, 0.0f, 0.0f},
			   {0.0f, 1.0f, 0.0f},
			   45.0f,
			   0};

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			Vector2 mouseDelta{GetMouseDelta()};
			//mouseDelta.y *= std::pow(
			//	1.0f - std::abs(Vector3DotProduct(
			//			   Vector3Normalize(cam.position), {0.0f, 1.0f, 0.0f})),
			//	2);

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

		vxl::DrawVolume(&model);

		EndMode3D();
		DrawFPS(0, 0);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
