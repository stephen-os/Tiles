#include "TilePreviewPanel.h"

#include "imgui.h"

#include "../Core/TileRenderer.h"
#include "Lumina/Core/Log.h"

namespace Tiles
{
	void TilePreviewPanel::OnUIRender()
	{
		ImGui::Begin("Tile Preview", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (!m_Atlas->HasTexture() || !m_TileAttributes->IsTextureSelected())
		{
			ImGui::Text("No texture selected.");
			ImGui::End();
			return;
		}

		glm::vec4 tintColor = m_TileAttributes->GetTintColor();
		glm::vec4 textureCords = m_Atlas->GetTextureCoords(m_TileAttributes->GetTextureIndex());

		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		float squareSize = std::min(contentRegion.x, contentRegion.y);

		// Calculate top-left position to center the square
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		float offsetX = (contentRegion.x - squareSize) * 0.5f;
		float offsetY = (contentRegion.y - squareSize) * 0.5f;
		ImVec2 imagePos = ImVec2(cursorPos.x + offsetX, cursorPos.y + offsetY);
		ImVec2 imageEnd = ImVec2(imagePos.x + squareSize, imagePos.y + squareSize);

		ImVec2 uv0(textureCords.x, textureCords.y);
		ImVec2 uv1(textureCords.z, textureCords.w);
		ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(tintColor.r, tintColor.g, tintColor.b, tintColor.a));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddImage((void*)(intptr_t)m_Atlas->GetTexture()->GetID(), imagePos, imageEnd, uv0, uv1, color);

		ImGui::End();
	}
}