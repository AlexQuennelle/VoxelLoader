#include "model.h"
#include "raylib.h"
#include <iostream>
#include <string>

int main()
{
	std::string fileName;
	std::cout << "Enter file name: ";
	std::cin >> fileName;
	InitWindow(800, 800, NAME);
	vxl::Model tea(RESOURCES_PATH + fileName);
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground({100, 149, 237, 255});
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
