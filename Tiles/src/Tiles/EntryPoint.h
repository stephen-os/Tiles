#pragma once

#include "Core/Application.h"

#ifdef TILES_PLATFORM_WINDOWS

extern Tiles::Application* Tiles::CreateApplication(int argc, char** argv);

namespace Tiles
{
	int Main(int argc, char** argv)
	{
		Tiles::Application* app = Tiles::CreateApplication(argc, argv);
		app->Create();
		app->Run();
		app->Destroy();
		delete app;

		return 0;
	}
}

#ifdef TILES_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Tiles::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Tiles::Main(argc, argv);
}

#endif // TILES_DIST

#endif // TILES_PLATFORM_WINDOWS
