#pragma once

#include <cstdint>

namespace Tiles::Input
{
	// Mouse button codes. Mirrors GLFW's mouse button codes.
	enum class MouseCode : uint16_t
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	};
}
