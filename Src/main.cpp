#include "model.h"
#include "raylib.h"
#include "volume.h"
#include <cstring>
#include <filesystem>
#include <iostream>
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
	if(!std::filesystem::exists(fileName))
	{
		return -1;
	}

	InitWindow(800, 800, NAME);

	vxl::Model model(RESOURCES_PATH + fileName);

	Camera cam{{150.0f, 150.0f, 150.0f},
			   {0.0f, 0.0f, 0.0f},
			   {0.0f, 1.0f, 0.0f},
			   45.0f,
			   0};

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground({100, 149, 237, 255});
		BeginMode3D(cam);

		vxl::DrawVolume(&model);

		EndMode3D();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
