#include "ViewportPanel.h"

#include "../Core/Tools.h"
#include "../Core/Color.h"
#include "../Core/Base.h"

#include "../Commands/ReplaceTileCommand.h"
#include "../Commands/ReplaceLayerCommand.h"

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
		
        TileRenderer::Init();
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        HandleMouseInput();
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
                    HandleSelection(m_Layers->GetActiveLayer(), y, x);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Handling
/////////////////////////////////////////////////////////////////////////////////////////////////////

    void ViewportPanel::HandleSelection(size_t l, size_t y, size_t x)
    {
        // Dont select if no layer is selected
		if (m_Layers->IsEmpty())
			return;

        static bool wasErasing = m_ToolSelection->Erase;
        static bool wasFilling = m_ToolSelection->Fill;

        if (wasErasing != m_ToolSelection->Erase || wasFilling != m_ToolSelection->Fill)
        {
            m_LastMousePos = { -1, -1 };
        }

        wasErasing = m_ToolSelection->Erase;
        wasFilling = m_ToolSelection->Fill;

        // Are we ont the active layer? 
        if (l != m_Layers->GetActiveLayer())
            return;

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
            Position position;
            position.index = l;
            position.y = y;
            position.x = x;

            Tile& oldTile = m_Layers->GetTile(l, y, x);

            Tile newTile;

            m_CommandHistory->ExecuteCommand(MakeUnique<ReplaceTileCommand>(position, oldTile, newTile));

            return;
        }

        // Is there a selection?
        if (!m_TextureSelection || m_TextureSelection->Empty())
            return;

        // For now we are only going to fill what is the first texture in the selection
        // otherwise paint with whole selection. 
        if (m_ToolSelection->Fill)
        {
            Tile& tile = m_Layers->GetTile(l, y, x);

            // Check tile to make sure its not the same. 
            if (tile.GetTextureIndex() == m_TextureSelection->Front())
                return;

            // If the tiles are different, then we ill
            Layer& oldLayer = m_Layers->GetLayer(l);
            Layer newLayer = oldLayer;
            Tools::Fill(newLayer, m_TextureSelection->Front(), y, x);
            m_CommandHistory->ExecuteCommand(MakeUnique<ReplaceLayerCommand>(l, oldLayer, newLayer));
        }
        else
        {
            int baseIndex = m_TextureSelection->Front();
            glm::vec2 basePos = m_Atlas->GetPosition(baseIndex);

            for (int texture : *m_TextureSelection)
            {
                glm::vec2 relativePos = m_Atlas->GetPosition(texture);
                glm::vec2 normalizedPos = relativePos - basePos;

                int targetX = x + (int)normalizedPos.x;
                int targetY = y + (int)normalizedPos.y;

                // Skip out-of-bounds tiles
                if (targetX < 0 || targetY < 0 || targetX >= m_Layers->GetWidth() || targetY >= m_Layers->GetHeight())
                    continue;

                Position position;
                position.index = l;
                position.y = targetY;
                position.x = targetX;

                Tile& oldTile = m_Layers->GetTile(l, targetY, targetX);

                // Check tile to make sure its not the same. 
                if (oldTile.GetTextureIndex() == texture)
                    return;

                Tile newTile;
                newTile.SetTextureIndex(texture);

                m_CommandHistory->ExecuteCommand(MakeUnique<ReplaceTileCommand>(position, oldTile, newTile));
            }
        }
    }

    void ViewportPanel::HandleMouseInput()
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        if (!IsMouseInViewport(mousePos, windowPos, windowSize))
            return;

        // The goal of this is to turn of viewport movement when we are in other menues and popups. 
		// We could some flag to check if we are in a popup.
		if (!ImGui::IsWindowFocused())
		    return;

        m_ViewportCamera.HandleKeyInput(0.01f);

        // Translating Screen
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

        // Zooming
        float scrollDelta = ImGui::GetIO().MouseWheel;
        if (scrollDelta != 0.0f)
            m_Zoom += scrollDelta;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utils
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ViewportPanel::IsNewClick()
    {
        return ImGui::IsMouseClicked(0) && !m_IsMouseDragging;
    }

    bool ViewportPanel::IsNewTileDuringDrag(glm::vec2 currentTilePos)
    {
        return ImGui::IsMouseDown(0) && m_IsMouseDragging &&
            (currentTilePos.x != m_LastMousePos.x || currentTilePos.y != m_LastMousePos.y);
    }

    bool ViewportPanel::IsMouseInViewport(const ImVec2& mousePos, const ImVec2& windowPos, const ImVec2& windowSize)
    {
        return (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
            mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);
    }
}