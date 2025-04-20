#include "TintSelectionPanel.h"

#include "imgui.h"

namespace Tiles
{
	void TintSelectionPanel::OnUIRender()
	{
		if (!m_TileAttributes)
			return;

		ImGui::Begin("Tint Selection", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		glm::vec4& tintColor = m_TileAttributes->GetTintColor();

		ImGui::Text("Tint Selection");
		ImGui::ColorPicker4("##TintColor", &tintColor[0], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueBar);

		if (ImGui::Button("Reset Tint"))
		{
			tintColor = glm::vec4(1.0f);
		}

		ImGui::End();
	}
}
