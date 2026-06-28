#include "PanelDebug.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Tiles
{
    PanelDebug::PanelDebug(Ref<Context> context)
        : Panel(context), m_LastUpdateTime(std::chrono::steady_clock::now())
    {
    }

    void PanelDebug::Update()
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_LastUpdateTime);
        m_UpdateTime = duration.count() / 1000.0f; // Convert to milliseconds
        m_LastUpdateTime = currentTime;
    }

    void PanelDebug::Render()
    {
        auto renderStart = std::chrono::steady_clock::now();

        if (!ImGui::Begin("Debug Panel"))
        {
            ImGui::End();
            return;
        }

        if (!m_Context)
        {
            ImGui::Text("No Context Available");
            ImGui::End();
            return;
        }

        // Render all debug sections
        if (ImGui::CollapsingHeader("Context Information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderContextInfo();
        }

        if (ImGui::CollapsingHeader("Project Information", ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderProjectInfo();
        }

        if (ImGui::CollapsingHeader("Layer Stack", ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderLayerStackInfo();
        }

        if (ImGui::CollapsingHeader("Working Layer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            RenderWorkingLayerInfo();
        }

        if (ImGui::CollapsingHeader("Painting State"))
        {
            RenderPaintingInfo();
        }

        if (ImGui::CollapsingHeader("Command History"))
        {
            RenderCommandHistoryInfo();
        }

        if (ImGui::CollapsingHeader("Texture Atlases"))
        {
            RenderTextureAtlasInfo();
        }

        if (ImGui::CollapsingHeader("Performance"))
        {
            RenderPerformanceInfo();
        }

        ImGui::End();

        // Calculate render time
        auto renderEnd = std::chrono::steady_clock::now();
        auto renderDuration = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart);
        m_RenderTime = renderDuration.count() / 1000.0f; // Convert to milliseconds
    }

    void PanelDebug::RenderContextInfo()
    {
        ImGui::Text("Context Address: %p", m_Context.get());
        ImGui::Text("Context Valid: %s", m_Context ? "Yes" : "No");
        ImGui::Text("Is Dirty: %s", m_Context->IsDirty() ? "Yes" : "No");
    }

    void PanelDebug::RenderProjectInfo()
    {
        const auto& project = m_Context->GetProject();

        ImGui::Text("Project Name: %s", project->GetProjectName().c_str());
        ImGui::Text("File Path: %s", project->GetFilePath().string().c_str());
        ImGui::Text("Is New Project: %s", project->IsNew() ? "Yes" : "No");
        ImGui::Text("Has Unsaved Changes: %s", project->HasUnsavedChanges() ? "Yes" : "No");

        ImGui::Text("Last Accessed: %s", FormatTimePoint(project->GetLastAccessed()).c_str());
        ImGui::Text("Last Saved: %s", FormatTimePoint(project->GetLastSaved()).c_str());
    }

    void PanelDebug::RenderLayerStackInfo()
    {
        const auto& layerStack = m_Context->GetProject()->GetLayerStack();

        ImGui::Text("Dimensions: %d x %d", layerStack.GetWidth(), layerStack.GetHeight());
        ImGui::Text("Layer Count: %zu", layerStack.GetLayerCount());
        ImGui::Text("Is Empty: %s", layerStack.IsEmpty() ? "Yes" : "No");

        if (layerStack.GetLayerCount() > 0)
        {
            ImGui::Separator();
            ImGui::Text("Layers:");

            for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
            {
                const auto& layer = layerStack.GetLayer(i);

                if (ImGui::TreeNode(("Layer " + std::to_string(i)).c_str()))
                {
                    ImGui::Text("  Name: %s", layer.GetName().c_str());
                    ImGui::Text("  Dimensions: %d x %d", layer.GetWidth(), layer.GetHeight());
                    ImGui::Text("  Visible: %s", layer.GetVisibility() ? "Yes" : "No");
                    ImGui::Text("  Render Group: %d", layer.GetRenderGroup());
                    ImGui::Text("  Rendering Enabled: %s", layer.IsRenderingEnabled() ? "Yes" : "No");
                    ImGui::Text("  Tile Count: %zu", layer.GetTileCount());
                    ImGui::Text("  Is Empty: %s", layer.IsEmpty() ? "Yes" : "No");

                    // Count painted tiles
                    size_t paintedTiles = 0;
                    for (const auto& tile : layer)
                    {
                        if (tile.IsPainted()) paintedTiles++;
                    }
                    ImGui::Text("  Painted Tiles: %zu", paintedTiles);

                    ImGui::TreePop();
                }
            }
        }
    }

    void PanelDebug::RenderWorkingLayerInfo()
    {
        ImGui::Text("Working Layer Index: %zu", m_Context->GetWorkingLayer());
        ImGui::Text("Has Working Layer: %s", m_Context->HasWorkingLayer() ? "Yes" : "No");

        if (m_Context->HasWorkingLayer())
        {
            const auto& workingLayer = m_Context->GetWorkingLayerRef();
            ImGui::Separator();
            ImGui::Text("Working Layer Details:");
            ImGui::Text("  Name: %s", workingLayer.GetName().c_str());
            ImGui::Text("  Dimensions: %d x %d", workingLayer.GetWidth(), workingLayer.GetHeight());
            ImGui::Text("  Visible: %s", workingLayer.GetVisibility() ? "Yes" : "No");
            ImGui::Text("  Render Group: %d", workingLayer.GetRenderGroup());
            ImGui::Text("  Tile Count: %zu", workingLayer.GetTileCount());

            // Count painted tiles in working layer
            size_t paintedTiles = 0;
            for (const auto& tile : workingLayer)
            {
                if (tile.IsPainted()) paintedTiles++;
            }
            ImGui::Text("  Painted Tiles: %zu", paintedTiles);
        }
    }

    void PanelDebug::RenderPaintingInfo()
    {
        ImGui::Text("Painting Mode: %s", GetPaintingModeString(m_Context->GetPaintingMode()));

        const auto& brush = m_Context->GetBrush();
        ImGui::Separator();
        ImGui::Text("Brush Information:");
        ImGui::Text("  Is Painted: %s", brush.IsPainted() ? "Yes" : "No");
        ImGui::Text("  Is Textured: %s", brush.IsTextured() ? "Yes" : "No");
        ImGui::Text("  Atlas Index: %zu", brush.GetAtlasIndex());
        ImGui::Text("  Has Valid Atlas: %s", brush.HasValidAtlas() ? "Yes" : "No");

        const auto& rotation = brush.GetRotation();
        ImGui::Text("  Rotation: (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);

        const auto& size = brush.GetSize();
        ImGui::Text("  Size: (%.2f, %.2f)", size.x, size.y);

        const auto& tint = brush.GetTint();
        ImGui::Text("  Tint: (%.2f, %.2f, %.2f, %.2f)", tint.r, tint.g, tint.b, tint.a);

        const auto& texCoords = brush.GetTextureCoords();
        ImGui::Text("  Texture Coords: (%.2f, %.2f, %.2f, %.2f)",
            texCoords.x, texCoords.y, texCoords.z, texCoords.w);
    }

    void PanelDebug::RenderCommandHistoryInfo()
    {
        ImGui::Text("Can Undo: %s", m_Context->CanUndo() ? "Yes" : "No");
        ImGui::Text("Can Redo: %s", m_Context->CanRedo() ? "Yes" : "No");

        if (ImGui::Button("Clear History"))
        {
            m_Context->ClearHistory();
        }
    }

    void PanelDebug::RenderTextureAtlasInfo()
    {
        auto& project = m_Context->GetProject();
        ImGui::Text("Atlas Count: %zu", project->GetTextureAtlasCount());

        if (project->GetTextureAtlasCount() > 0)
        {
            ImGui::Separator();
            for (size_t i = 0; i < project->GetTextureAtlasCount(); ++i)
            {
                auto atlas = project->GetTextureAtlas(i);
                if (atlas)
                {
                    ImGui::Text("Atlas %zu: Valid", i);
                    // Add more atlas-specific info if TextureAtlas has accessible properties
                }
                else
                {
                    ImGui::Text("Atlas %zu: Invalid", i);
                }
            }
        }
    }

    void PanelDebug::RenderPerformanceInfo()
    {
        ImGui::Text("Update Time: %.3f ms", m_UpdateTime);
        ImGui::Text("Render Time: %.3f ms", m_RenderTime);
        ImGui::Text("Total Frame Time: %.3f ms", m_UpdateTime + m_RenderTime);

        ImGui::Separator();
        ImGui::Text("Memory Usage (approximate):");

        const auto& project = m_Context->GetProject();
        const auto& layerStack = project->GetLayerStack();

        size_t totalTiles = 0;
        for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
        {
            totalTiles += layerStack.GetLayer(i).GetTileCount();
        }

        size_t tileMemory = totalTiles * sizeof(Tile);
        ImGui::Text("  Tiles: %zu (%zu bytes)", totalTiles, tileMemory);
        ImGui::Text("  Layers: %zu", layerStack.GetLayerCount());
        ImGui::Text("  Atlases: %zu", project->GetTextureAtlasCount());
    }

    std::string PanelDebug::FormatTimePoint(const std::chrono::steady_clock::time_point& timePoint) const
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - timePoint);

        if (duration.count() == 0)
        {
            return "Just now";
        }

        return std::to_string(duration.count()) + " seconds ago";
    }

    const char* PanelDebug::GetPaintingModeString(PaintingMode mode) const
    {
        switch (mode)
        {
        case PaintingMode::None:   return "None";
        case PaintingMode::Brush:  return "Brush";
        case PaintingMode::Eraser: return "Eraser";
        case PaintingMode::Fill:   return "Fill";
        default:                   return "Unknown";
        }
    }
}