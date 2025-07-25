#include "utils.h"
#include "vxlModel.h"

#include <algorithm>
#include <cmath>
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

void Update();

Camera cam;
vxl::vxlModel* vxlmodel;
Vector2 camSpeed;

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
			if (name.substr(name.length() - 4, 4) == ".vox")
			{
				files.push_back(name);
				std::cout << name << '\n';
			}
		}
		std::cout << files.size() << '\n';
		std::uniform_int_distribution<> random(0, files.size() - 1);
		fileName = files[random(gen)];
	}

	InitWindow(500, 500, NAME);
#else
	std::string fileName;
	std::cout << "Chose a file to load\n\n";
	{
		namespace fs = std::filesystem;
		for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
		{
			std::string name =
				entry.path().string().erase(0, strlen(RESOURCES_PATH));
			if (name.substr(name.length() - 4, 4) == ".vox")
			{
				std::cout << '\t' << name << '\n';
			}
		}
	}
	std::cout << "\n\n\0337";
	std::cout << "Enter file name: ";
	bool chosingFile{true};
	while (chosingFile)
	{
		std::cin >> fileName;

		if (!std::filesystem::exists(RESOURCES_PATH + fileName))
		{
			//Move back up one line to counteract RETURN and clear
			//Then restore saved position and move up one
			//This means the error message prints above the input and the
			//input field clears itself correctly regardless of empty returns
			std::cout << "\033[1A\033[2K\0338\033[1A\033[2K";
			vxl::SetTextColor(vxl::ERROR);
			std::cout << RESOURCES_PATH << fileName << " does not exist\0338";
			std::cout << "Enter file name: ";
			vxl::ClearStyles();
		}
		else if (fileName.substr(fileName.length() - 4, 4) != ".vox")
		{
			std::cout << "\033[1A\033[2K\0338\033[1A";
			vxl::SetTextColor(vxl::ERROR);
			std::cout << RESOURCES_PATH << fileName
					  << " is not a .vox file\0338";
			std::cout << "Enter file name: ";
			vxl::ClearStyles();
		}
		else
		{
			chosingFile = false;
			std::cout << "\033[2J\033[H";
		}
	}

	InitWindow(800, 800, NAME);
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

	vxlmodel->Draw();

	EndMode3D();
	DrawFPS(0, 0);
	EndDrawing();
}
