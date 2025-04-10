#include "raylib.h"

int main()
{
	InitWindow(800, 800, "test");
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
