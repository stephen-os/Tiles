#pragma once

#include "imgui.h"

namespace Tiles
{
	struct Color
	{
        static constexpr ImU32 FILL_COLOR = IM_COL32(255, 255, 255, 255);               // White
        static constexpr ImU32 BACKGROUND_COLOR = IM_COL32(77, 77, 77, 255);            // Dark Gray
        static constexpr ImU32 OUTLINE_COLOR = IM_COL32(169, 169, 169, 255);            // Lighter Gray
        static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255);     // Orange
        static constexpr ImU32 DEAULT_BORDER_COLOR = IM_COL32(51, 51, 51, 255);         // Black

        static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255);     // Light Gray
        static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);        // Dark Gray

	};
}