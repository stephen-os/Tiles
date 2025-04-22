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
		glm::vec4 textureCoords = m_Atlas->GetTextureCoords(m_TileAttributes->GetTextureIndex());
		float rotation = m_TileAttributes->GetRotation(); // In radians

		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		float squareSize = std::min(contentRegion.x, contentRegion.y);

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		float offsetX = (contentRegion.x - squareSize) * 0.5f;
		float offsetY = (contentRegion.y - squareSize) * 0.5f;
		ImVec2 center = ImVec2(cursorPos.x + offsetX + squareSize * 0.5f, cursorPos.y + offsetY + squareSize * 0.5f);

		// Precompute sine/cosine
		float cosAngle = std::cos(rotation);
		float sinAngle = std::sin(rotation);

		// Define local corners of the square (centered at origin)
		ImVec2 localCorners[4] = {
			{ -0.5f, -0.5f }, // top-left
			{  0.5f, -0.5f }, // top-right
			{  0.5f,  0.5f }, // bottom-right
			{ -0.5f,  0.5f }  // bottom-left
		};

		// Rotate and translate corners to screen space
		ImVec2 worldCorners[4];
		for (int i = 0; i < 4; ++i)
		{
			float x = localCorners[i].x * squareSize;
			float y = localCorners[i].y * squareSize;

			float rotatedX = x * cosAngle - y * sinAngle;
			float rotatedY = x * sinAngle + y * cosAngle;

			worldCorners[i] = ImVec2(center.x + rotatedX, center.y + rotatedY);
		}

		ImVec2 uv0(textureCoords.x, textureCoords.y);
		ImVec2 uv1(textureCoords.z, textureCoords.w);

		ImVec2 uvs[4] = {
			{ uv0.x, uv0.y }, // top-left
			{ uv1.x, uv0.y }, // top-right
			{ uv1.x, uv1.y }, // bottom-right
			{ uv0.x, uv1.y }  // bottom-left
		};

		ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(tintColor.r, tintColor.g, tintColor.b, tintColor.a));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddImageQuad(
			(void*)(intptr_t)m_Atlas->GetTexture()->GetID(),
			worldCorners[0], worldCorners[1], worldCorners[2], worldCorners[3],
			uvs[0], uvs[1], uvs[2], uvs[3],
			color
		);

		ImGui::End();
	}
}