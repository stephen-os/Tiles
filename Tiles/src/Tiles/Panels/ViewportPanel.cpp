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

namespace Tiles
{

    ViewportPanel::ViewportPanel()
    {
        m_ViewportCamera.SetPosition({ 0.9f, 0.9f, -1.0f });
     
        std::string vertexShaderSource = Lumina::ReadFile("res/shaders/Background.vert");
        std::string fragmentShaderSource = Lumina::ReadFile("res/shaders/Background.frag");
        Shared<Lumina::ShaderProgram> shader = Lumina::ShaderProgram::Create(vertexShaderSource, fragmentShaderSource);

        m_BackgroundAttributes.Shader = shader;
		m_BackgroundAttributes.Size = { 5.0f, 5.0f };

        m_TileAttributes.Position = { 0.02f, 0.02f, 0.0f };
		m_TileAttributes.Size = { 0.02f, 0.02f };
    }

    void ViewportPanel::OnUIRender()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        // 10 zoom for now until we fix that

        Render(); 

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));         // Fully transparent button
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));  // Transparent when hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 0.0f));   // Transparent when clicked
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));         // Transparent border by default
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));   // Transparent border shadow

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);                         // Remove roundness
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);                       // Border thickness

        glm::vec2 cameraPos = { m_ViewportCamera.GetPosition().x, m_ViewportCamera.GetPosition().y };
        cameraPos = cameraPos * m_ViewportSize;
        
		glm::vec2 center = { m_ViewportSize.x / 2.0f, m_ViewportSize.y / 2.0f };
	
        ImGui::SetCursorPos(ImVec2(center.x, center.y));
        for (size_t y = 0; y < m_Layers->GetWidth(); y++)
        {
            for (size_t x = 0; x < m_Layers->GetHeight(); x++)
            {
                ImVec2 buttonSize = ImVec2(m_TileSize / m_Zoom, m_TileSize / m_Zoom);
                ImGui::SetCursorPos(ImVec2(center.x - (cameraPos.x * 0.5) + (x * buttonSize.x), center.y - (cameraPos.y * 0.5) + (y * buttonSize.y)));
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
                        ImGui::GetWindowDrawList()->AddImage((void*)m_Atlas->GetTextureID(), tileMin, tileMax, uvMin, uvMax, Color::FILL_COLOR);
                    }
                }

                // ImGui::GetWindowDrawList()->AddRectFilled(tileMin, tileMax, IM_COL32_BLACK);
            }
        }

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);

        ImGui::End();

        ImGui::Begin("Statistic");
        Lumina::Renderer::Statistics stats = Lumina::Renderer::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad Count: %d", stats.QuadCount);
        Lumina::Renderer::ResetStats();
        ImGui::End();
    }

    void ViewportPanel::Render()
    {
		// No layers, skip completely
		if (!m_Layers)
			return; 

        m_ViewportCamera.SetProjectionMatrix(1.0f * m_Zoom, m_ViewportSize.x / m_ViewportSize.y, 0.01f, 100.0f);
        m_ViewportCamera.HandleKeyInput(0.01f);

        Lumina::Renderer::Begin(m_ViewportCamera);
        Lumina::Renderer::SetResolution(m_ViewportSize.x, m_ViewportSize.y);

        m_BackgroundAttributes.Shader->Bind();
        // * 4.0f because 4 small squares make 1 big square
        m_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * m_Layers->GetWidth(), 4.0f * m_Layers->GetHeight()});
        m_BackgroundAttributes.Shader->Unbind();

        // Draw Background
        Lumina::Renderer::DrawQuad(m_BackgroundAttributes);

        // No atlas, just render the background 
		if (!m_Atlas)
			return;

        m_TileAttributes.Texture = m_Atlas->GetTexture();

		// Draw Tiles
		for (size_t layerIndex = 0; layerIndex < m_Layers->GetSize(); layerIndex++)
		{
			Layer& layer = m_Layers->GetLayer(layerIndex);
			for (size_t y = 0; y < layer.GetHeight(); y++)
			{
				for (size_t x = 0; x < layer.GetWidth(); x++)
				{
					Tile& tile = layer.GetTile(y, x);
					if (tile.GetTextureIndex() == -1)
						continue;
					m_TileAttributes.Position = { x * 0.04f + 0.02f, y * 0.04f + 0.02f, 0.0f };
					m_TileAttributes.TextureCoords = m_Atlas->GetTextureCoords(tile.GetTextureIndex());
					// spdlog::info("Drawing Tile: {} {} {}", layerIndex, y, x);
					Lumina::Renderer::DrawQuad(m_TileAttributes);
				}
			}
		}
        
        Lumina::Renderer::End();

        ImGui::Image((void*)(intptr_t)Lumina::Renderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));
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

                // m_Camera.Drag(mouseDelta);
				m_ViewportCamera.Advance(mouseDelta.x);
				m_ViewportCamera.Strafe(mouseDelta.y);

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