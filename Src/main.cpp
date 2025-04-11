#include "model.h"
#include "raylib.h"

int main()
{
	InitWindow(800, 800, NAME);
	vxl::Model tea(RESOURCES_PATH "dragon.vox");
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground({100, 149, 237, 255});
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
