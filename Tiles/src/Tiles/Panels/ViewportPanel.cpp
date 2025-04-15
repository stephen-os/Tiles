#include "ViewportPanel.h"

#include "../Core/Tools.h"
#include "../Core/Color.h"
#include "../Core/Base.h"

#include "../Commands/ReplaceTileCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

#include "PanelUtilities.h"

#include "Lumina/Utils/FileReader.h"

#include <algorithm>
#include <array>

#include <iostream>

#include <spdlog/spdlog.h>

#include "../Core/TileRenderer.h"; 

namespace Tiles
{

    ViewportPanel::ViewportPanel()
    {
        m_ViewportCamera.SetPosition({ 0.9f, 0.9f, -1.0f });
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        HandleInput();

		TileRenderer::Render(m_ViewportCamera, m_Layers, m_Atlas, m_ViewportSize, m_Zoom);

        ImGui::Image(TileRenderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));

        RenderPaintingOverlay();

        ImGui::End();
    }

    void ViewportPanel::RenderPaintingOverlay()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));         // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));   // Transparent when clicked
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));         // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // Transparent border shadow

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);                         // Remove roundness
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        glm::vec2 viewportCenter = { m_ViewportSize.x * 0.5f, m_ViewportSize.y * 0.5f };
        glm::vec2 cameraPos = { m_ViewportCamera.GetPosition().x * viewportCenter.x, m_ViewportCamera.GetPosition().y * viewportCenter.y };

        ImGui::SetCursorPos(ImVec2(viewportCenter.x, viewportCenter.y));
        for (size_t y = 0; y < m_Layers->GetWidth(); y++)
        {
            for (size_t x = 0; x < m_Layers->GetHeight(); x++)
            {
                ImVec2 buttonSize = ImVec2(m_TileSize / m_Zoom, m_TileSize / m_Zoom);
                ImGui::SetCursorPos(ImVec2(viewportCenter.x - (cameraPos.x) + (x * buttonSize.x), viewportCenter.y - (cameraPos.y) + (y * buttonSize.y)));
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();

                ImVec2 tileMin(cursorPos.x, cursorPos.y);
                ImVec2 tileMax(tileMin.x + buttonSize.x, tileMin.y + buttonSize.y);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    HandleSelection(y, x);

                    for (int texture : *m_TextureSelection)
                    {
                        glm::vec4 texCoords = m_Atlas->GetTextureCoords(texture);
                        ImVec2 uvMin(texCoords.x, texCoords.y);
                        ImVec2 uvMax(texCoords.z, texCoords.w);

                        ImGui::GetWindowDrawList()->AddRect(tileMin, tileMax, Color::SELECTION_BORDER_COLOR);
                        
                        if (m_Atlas->HasTexture())
                        {
                            uint32_t textureID = m_Atlas->GetTexture()->GetID();
                            ImGui::GetWindowDrawList()->AddImage((void*)textureID, tileMin, tileMax, uvMin, uvMax, Color::FILL_COLOR);
                        }
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

		uint32_t layerIndex = m_Layers->GetActiveLayer();

        static bool wasErasing = m_ToolSelection->Erase;
        static bool wasFilling = m_ToolSelection->Fill;

        if (wasErasing != m_ToolSelection->Erase || wasFilling != m_ToolSelection->Fill)
        {
            m_LastMousePos = { -1, -1 };
        }

        wasErasing = m_ToolSelection->Erase;
        wasFilling = m_ToolSelection->Fill;

        // Is this a new click?
        glm::vec2 currentTilePos(y, x);
        if (!IsNewClick() && !IsNewTileDuringDrag(currentTilePos))
            return;

        // Are we dragging?
        m_LastMousePos = currentTilePos;
        if (IsNewClick())
            m_IsMouseDragging = true;

        // If we are erasing, that is all we will do in this method. 
        if (m_ToolSelection->Erase)
        {
			Tools::Erase(m_Layers, m_CommandHistory, layerIndex, y, x);
            return;
        }

        // Is there a selection?
        if (!m_TextureSelection || m_TextureSelection->Empty())
            return;

        if (m_ToolSelection->Fill)
        {
			Tools::Fill(m_Layers, m_TextureSelection, m_CommandHistory, layerIndex, y, x);
        }
        else
        {
			Tools::Paint(m_Layers, m_Atlas, m_TextureSelection, m_CommandHistory, layerIndex, y, x);
        }
    }

    void ViewportPanel::HandleInput()
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

		// Do nothing if we are not in the viewport and the viewport isnt focused
        if (!PanelUtils::IsMouseInViewport(mousePos, windowPos, windowSize) && !ImGui::IsWindowFocused())
            return;

        // Translate Camera with keys
        m_ViewportCamera.HandleKeyInput(0.01f);

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

				m_ViewportCamera.Advance(-mouseDelta.y * 0.002f);
				m_ViewportCamera.Strafe(-mouseDelta.x * 0.002f);

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
            m_Zoom += delta;
        }
    }

    bool ViewportPanel::IsNewClick()
    {
        return ImGui::IsMouseClicked(0) && !m_IsMouseDragging;
    }

    bool ViewportPanel::IsNewTileDuringDrag(glm::vec2 currentTilePos)
    {
        return ImGui::IsMouseDown(0) && m_IsMouseDragging &&
            (currentTilePos.x != m_LastMousePos.x || currentTilePos.y != m_LastMousePos.y);
    }
}