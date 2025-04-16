#include "ViewportPanel.h"

#include "../Core/Tools.h"
#include "../Core/Color.h"
#include "../Core/Base.h"
#include "../Core/Tile.h"

#include "../Commands/ReplaceTileCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

#include "PanelUtilities.h"

#include "Lumina/Utils/FileReader.h"
#include "Lumina/Renderer/RenderCommands.h"

#include "../Core/TileRenderer.h"

namespace Tiles
{
    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        HandleInput();

		TileRenderer::SetResolution(m_ViewportSize);

        TileRenderer::Begin();
        TileRenderer::DrawGrid(m_Layers);
		TileRenderer::DrawLayers(m_Layers, m_Atlas);
 
		RenderOverlay();

        ImGui::SetCursorPos({ 0.0f, 0.0f });
        ImGui::Image(TileRenderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));
        TileRenderer::End();
        // Set cursor back to top left corner
        
        ImGui::End();
    }

    void ViewportPanel::RenderOverlay()
    {
        if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
            return;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        // Do nothing if we are not in the viewport and the viewport isnt focused
        if (!PanelUtils::IsMouseInViewport(mousePos, windowPos, windowSize) && !ImGui::IsWindowFocused())
            return;

        // Overlay
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));         // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));   // Transparent when clicked
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));         // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // Transparent border shadow

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);                         // Remove roundness
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        glm::vec2 viewportCenter = { m_ViewportSize.x * 0.5f, m_ViewportSize.y * 0.5f };
        Lumina::Camera& camera = TileRenderer::GetCamera();
        glm::vec2 cameraPos = { camera.GetPosition().x * viewportCenter.x, camera.GetPosition().y * viewportCenter.y };

        for (size_t y = 0; y < m_Layers->GetWidth(); y++)
        {
            for (size_t x = 0; x < m_Layers->GetHeight(); x++)
            {
                float zoom = TileRenderer::GetZoom();
                ImVec2 buttonSize = ImVec2(m_TileSize / zoom, m_TileSize / zoom);
                ImGui::SetCursorPos(ImVec2(viewportCenter.x - (cameraPos.x) + (x * buttonSize.x), viewportCenter.y - (cameraPos.y) + (y * buttonSize.y)));
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();

                ImVec2 tileMin(cursorPos.x, cursorPos.y);
                ImVec2 tileMax(tileMin.x + buttonSize.x, tileMin.y + buttonSize.y);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        HandleSelection(y, x);

                    ImGui::GetForegroundDrawList()->AddRect(tileMin, tileMax, Color::SELECTION_BORDER_COLOR, 0.0f, 0, 2.0f);

                    for (int texture : *m_TextureSelection)
                    {
                        Tile tile;
                        tile.SetTextureIndex(texture);
                        // TileRenderer::DrawTile(tile, m_Atlas, { x, y });
                    }

                }
            }
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
    }

    void ViewportPanel::HandleSelection(size_t y, size_t x)
    {
        // Dont select if no layer is selected
		if (m_Layers->IsEmpty())
			return;

        // If we are erasing, that is all we will do in this method. 
        if (m_ToolSelection->Erase)
        {
			Tools::Erase(m_Layers, m_CommandHistory, y, x);
            return;
        }

        // Is there a selection?
        if (!m_TextureSelection || m_TextureSelection->Empty())
            return;

        if (m_ToolSelection->Fill)
        {
			Tools::Fill(m_Layers, m_TextureSelection, m_CommandHistory, y, x);
        }
        else
        {
			Tools::Paint(m_Layers, m_Atlas, m_TextureSelection, m_CommandHistory, y, x);
        }
    }

    void ViewportPanel::HandleInput()
    {
        ImVec2 mousePos = ImGui::GetMousePos();

		Lumina::Camera& camera = TileRenderer::GetCamera();

        // Translate Camera with keys
        camera.HandleKeyInput(0.01f);

		// Transelate Camera with mouse
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            if (!m_IsMiddleMouseDown)
            {
                m_IsMiddleMouseDown = true;
                m_LastMousePos = glm::vec2(mousePos.x, mousePos.y);
            }
            else
            {
                glm::vec2 currentMousePos(mousePos.x, mousePos.y);
                glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

                camera.Advance(-mouseDelta.y * 0.002f);
                camera.Strafe(-mouseDelta.x * 0.002f);

                m_LastMousePos = currentMousePos;
            }
        }
        else
        {
            m_IsMiddleMouseDown = false;
        }

        // Zoom Camera
        float delta = ImGui::GetIO().MouseWheel;
        if (delta != 0.0f)
        {
            TileRenderer::Zoom(delta);
        }
    }
}