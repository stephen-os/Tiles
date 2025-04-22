#pragma once

#include "imgui.h"
#include "glm/glm.hpp"

namespace Tiles
{
	namespace PanelUtils
	{
		static bool IsMouseInViewport()
		{
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 mousePos = ImGui::GetMousePos();

			return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
				mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
		}

		static bool IsPopupOpen()
		{
			return ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId);
		}
	} 
}