#pragma once

#include "imgui.h"
#include "glm/glm.hpp"

namespace Tiles
{
	namespace PanelUtils
	{


		static bool IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize)
		{
			return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
				mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
		}
	} 
}