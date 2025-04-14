#include "model.h"
#include "raylib.h"
#include "volume.h"
#include <iostream>
#include <string>

int main()
{
	std::string fileName;
	std::cout << "Enter file name: ";
	std::cin >> fileName;
	//fileName = "dragon.vox";

	InitWindow(800, 800, NAME);

	vxl::Model tea(RESOURCES_PATH + fileName);

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

		vxl::DrawVolume(&tea);

		EndMode3D();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
