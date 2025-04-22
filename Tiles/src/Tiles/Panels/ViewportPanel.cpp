#include "ViewportPanel.h"

#include "../Core/Tile.h"
#include "../Core/Color.h"
#include "../Core/Selection.h"
#include "../Core/TileRenderer.h"

#include "../Commands/EraseTileCommand.h"
#include "../Commands/PaintTileCommand.h"
#include "../Commands/FillTileCommand.h"

#include "PanelUtilities.h"

#include "Lumina/Core/Aliases.h"
#include "Lumina/Core/Log.h"

#include "Lumina/Utils/FileReader.h"

#include "Lumina/Renderer/RenderCommands.h"

namespace Tiles
{
    void ViewportPanel::Render()
    {
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		TileRenderer::SetResolution(m_ViewportSize);

        TileRenderer::Begin();
        TileRenderer::DrawGrid(*m_Layers);
        TileRenderer::DrawLayers(*m_Layers, *m_Atlas);
		
        if (PanelUtils::IsMouseInViewport() && !PanelUtils::IsPopupOpen())
        {
            HandleInput();
            RenderOverlay();
        }

        TileRenderer::End();

        ImGui::SetCursorPos({ 0.0f, 0.0f });
        ImGui::Image(TileRenderer::GetImage(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));

        ImGui::End();
    }

    void ViewportPanel::RenderOverlay()
    {
        glm::vec2 viewportCenter = { m_ViewportSize.x * 0.5f, m_ViewportSize.y * 0.5f };
        Lumina::Camera& camera = TileRenderer::GetCamera();
        glm::vec2 cameraPos = { camera.GetPosition().x * viewportCenter.x, camera.GetPosition().y * viewportCenter.y };

        for (size_t row = 0; row < m_Layers->GetHeight(); row++)
        {
            for (size_t col = 0; col < m_Layers->GetWidth(); col++)
            {
                float zoom = TileRenderer::GetZoom();
                ImVec2 buttonSize = ImVec2(m_TileSize / zoom, m_TileSize / zoom);
                ImGui::SetCursorPos(ImVec2(
                    viewportCenter.x - (cameraPos.x) + (col * buttonSize.x),
                    viewportCenter.y - (cameraPos.y) + (row * buttonSize.y)
                ));
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();

                ImVec2 tileMin(cursorPos.x, cursorPos.y);
                ImVec2 tileMax(tileMin.x + buttonSize.x, tileMin.y + buttonSize.y);

                if (ImGui::IsMouseHoveringRect(tileMin, tileMax))
                {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !m_Layers->IsEmpty())
                        HandleSelection(row, col);

                    ImGui::GetForegroundDrawList()->AddRect(tileMin, tileMax, Color::SELECTION_BORDER_COLOR, 0.0f, 0, 2.0f);

                    if (Selection::GetCurrentMode() == Selection::Mode::Paint)
                    {
                        TileRenderer::DrawTile(m_TileAttributes->GetTile(), *m_Atlas, row, col );
                    }
                }
            }
        }
    }

    void ViewportPanel::HandleSelection(size_t row, size_t col)
    {
	    TilePosition position(m_Layers->GetActiveLayer(), row, col);
        switch (Selection::GetCurrentMode())
        {
        case Selection::Mode::Paint:
        {
            m_CommandHistory->ExecuteCommand(MakeUnique<PaintTileCommand>(position, *m_Layers, *m_TileAttributes));
            break;
        }
        case Selection::Mode::Erase:
        {
            m_CommandHistory->ExecuteCommand(MakeUnique<EraseTileCommand>(position, *m_Layers));
            break;
        }
		case Selection::Mode::Fill:
        {
            m_CommandHistory->ExecuteCommand(MakeUnique<FillTileCommand>(position, *m_TileAttributes));
            break;
        }
		default: break;
        }
    }

    void ViewportPanel::HandleInput()
    {
        ImVec2 mousePos = ImGui::GetMousePos();
		Lumina::Camera& camera = TileRenderer::GetCamera();

        // Handle Shortcuts
        if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && ImGui::GetIO().KeyCtrl)
        {
            if (m_CommandHistory->CanUndo())
            {
                m_CommandHistory->Undo();
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Y, false) && ImGui::GetIO().KeyCtrl)
        {
            if (m_CommandHistory->CanRedo())
            {
                m_CommandHistory->Redo();
            }
        }

        // Translate Camera with keys
        camera.HandleKeyInput(0.01f);

		// Transelate Camera with mouse
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            glm::vec2 currentMousePos(mousePos.x, mousePos.y);
            glm::vec2 mouseDelta = currentMousePos - m_LastMousePos;

            camera.Advance(-mouseDelta.y * 0.002f);
            camera.Strafe(-mouseDelta.x * 0.002f);

            m_LastMousePos = currentMousePos;
        }
        else
        {
            m_LastMousePos = glm::vec2(mousePos.x, mousePos.y);
        }

        // Zoom Camera
        float delta = ImGui::GetIO().MouseWheel;
        if (delta != 0.0f)
        {
            TileRenderer::Zoom(delta);
        }
    }
}