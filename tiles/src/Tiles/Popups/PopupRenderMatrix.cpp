#include "PopupRenderMatrix.h"
#include "Core/Constants.h"
#include "Lumina/Lumina.h"
#include "Lumina/Core/Log.h"
#include "ImGuiFileDialog.h"
#include <algorithm>
#include <sstream>
#include <set>

using namespace Lumina;

namespace Tiles
{
    PopupRenderMatrix::PopupRenderMatrix(Ref<Context> context) : Popup(context) {}

    void PopupRenderMatrix::OnRender()
    {
        if (m_FirstShow)
        {
            InitializeDialog();
            m_FirstShow = false;
        }

        ImGui::SetNextWindowSizeConstraints(ImVec2(600, 0), ImVec2(600, FLT_MAX));
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Export Render Matrix", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!m_Context || !m_Context->HasProject())
            {
                ImGui::Text("No project loaded!");
                ImGui::End();
                return;
            }

            RenderLayerMatrix();
            ImGui::Separator();
            RenderExportSettings();
            ImGui::Separator();
            RenderActionButtons();
        }
        ImGui::End();

        if (m_ShowDirectorySelector)
        {
            ShowDirectoryDialog();
        }

        if (!m_IsVisible)
        {
            m_FirstShow = true;
        }
    }

    void PopupRenderMatrix::OnUpdate()
    {
    }

    void PopupRenderMatrix::InitializeDialog()
    {
        if (m_Context && m_Context->HasProject())
        {
            const auto& project = m_Context->GetProject();
            std::string projectName = project->GetProjectName();
            std::transform(projectName.begin(), projectName.end(), projectName.begin(), ::tolower);
            std::replace(projectName.begin(), projectName.end(), ' ', '_');

            m_ExportFileName = projectName;

            const auto& layerStack = project->GetLayerStack();
            m_LayerToRenderGroup.clear();

            for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
            {
                const auto& layer = layerStack.GetLayer(i);
                int renderGroupValue = static_cast<int>(layer.GetRenderGroup());
                m_LayerToRenderGroup[i] = std::max(-1, std::min(renderGroupValue, 99));
            }

            LUMINA_LOG_INFO("PopupRenderMatrix::InitializeDialog: Initialized dialog for project '{}'", projectName);
        }
    }

    void PopupRenderMatrix::RenderLayerMatrix()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        const auto& layerStack = m_Context->GetProject()->GetLayerStack();
        size_t layerCount = layerStack.GetLayerCount();

        ImGui::Text("Assign Layers to Render Groups");
        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

        float cell_width = 0.0f;
        float item_width = 0.0f;
        float cursor_x = 0.0f;

        if (ImGui::BeginTable("RenderMatrix", 7, tableFlags)) // 2 + 5 render groups
        {
            ImGui::TableSetupColumn("LayerName", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("LayerVisibility", ImGuiTableColumnFlags_WidthFixed, 25.0f);
            ImGui::TableSetupColumn("Disabled", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Background", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Midground", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Foreground", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Debug", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            // Layer Name header
            ImGui::TableSetColumnIndex(0);
            {
                const char* header = "Layer Name";
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header);
            }

            // Visibility header
            ImGui::TableSetColumnIndex(1);
            {
                const char* header = "Vis";
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header);
            }

            // Render group headers
            const char* groupNames[] = { "Disabled", "Background", "Midground", "Foreground", "Debug" };
            for (int i = 0; i < 5; ++i)
            {
                ImGui::TableSetColumnIndex(i + 2);
                const char* header = groupNames[i];
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header);
            }

            // Layer rows
            for (size_t layerIdx = 0; layerIdx < layerCount; ++layerIdx)
            {
                const auto& layer = layerStack.GetLayer(layerIdx);
                ImGui::TableNextRow();

                // Layer name
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(layer.GetName().c_str()).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::Text("%s", layer.GetName().c_str());

                // Visibility checkbox
                ImGui::TableSetColumnIndex(1);
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::GetFrameHeight();
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                ImGui::SetCursorPosX(cursor_x);

                bool visible = layer.GetVisibility();
                ImGui::BeginDisabled(true);
                ImGui::Checkbox(("##vis" + std::to_string(layerIdx)).c_str(), &visible);
                ImGui::EndDisabled();

                // Render group radio buttons
                int currentLayerGroup = static_cast<int>(layer.GetRenderGroup());
                int renderGroupValues[] = { -1, 0, 1, 2, 99 }; // Disabled, Background, Midground, Foreground, Debug

                for (int i = 0; i < 5; ++i)
                {
                    ImGui::TableSetColumnIndex(2 + i);

                    cell_width = ImGui::GetColumnWidth();
                    item_width = ImGui::GetFrameHeight();
                    cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                    ImGui::SetCursorPosX(cursor_x);

                    int groupValue = renderGroupValues[i];
                    bool isInGroup = m_LayerToRenderGroup[layerIdx] == groupValue;

                    // Highlight if this is the layer's actual current group but different from selected
                    if (currentLayerGroup == groupValue && currentLayerGroup != m_LayerToRenderGroup[layerIdx])
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                    }

                    if (ImGui::RadioButton(("##" + std::to_string(layerIdx) + "_" + std::to_string(groupValue)).c_str(), isInGroup))
                    {
                        m_LayerToRenderGroup[layerIdx] = groupValue;
                    }

                    if (currentLayerGroup == groupValue && currentLayerGroup != m_LayerToRenderGroup[layerIdx])
                    {
                        ImGui::PopStyleColor();
                    }
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    void PopupRenderMatrix::RenderExportSettings()
    {
        ImGui::Text("Export Settings");
        ImGui::Spacing();

        ImGui::Text("Export Directory:");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", m_ExportDirectory.string().c_str());
        ImGui::SameLine();
        if (ImGui::Button("Browse##Dir"))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_Modal;
            config.countSelectionMax = 1;

            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseExportDirDlg",
                "Choose Export Directory",
                nullptr,
                config
            );

            m_ShowDirectorySelector = true;
        }

        ImGui::Text("Base File Name:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        if (ImGui::InputText("##FileName", m_ExportFileName.data(), m_ExportFileName.capacity() + 1))
        {
            m_ExportFileName.resize(strlen(m_ExportFileName.data()));
        }

        ImGui::Text("Format:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        const char* formatNames[] = { "PNG", "JPG", "BMP", "TGA" };
        int currentFormat = static_cast<int>(m_ExportFormat);
        if (ImGui::Combo("##Format", &currentFormat, formatNames, IM_ARRAYSIZE(formatNames)))
        {
            m_ExportFormat = static_cast<ExportFormat>(currentFormat);
        }

        ImGui::Spacing();
        ImGui::Text("Layer Visibility:");
        ImGui::SameLine();
        ImGui::Checkbox("Visible", &m_ExportVisible);
        ImGui::SameLine();
        ImGui::Checkbox("Invisible", &m_ExportInvisible);

        ImGui::Checkbox("Use Project Bounds", &m_UseProjectBounds);
    }

    void PopupRenderMatrix::RenderActionButtons()
    {
        float buttonWidth = 100.0f;
        float totalWidth = buttonWidth * 4 + ImGui::GetStyle().ItemSpacing.x * 3;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        if (ImGui::Button("Apply", ImVec2(buttonWidth, 0)))
        {
            ApplyRenderGroupChanges();
        }

        ImGui::SameLine();
        if (ImGui::Button("Export", ImVec2(buttonWidth, 0)))
        {
            ApplyRenderGroupChanges();
            ExecuteExport();
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset", ImVec2(buttonWidth, 0)))
        {
            ResetToDefaults();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            Hide();
        }
    }

    void PopupRenderMatrix::ShowDirectoryDialog()
    {
        if (ImGuiFileDialog::Instance()->Display("ChooseExportDirDlg"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                m_ExportDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
            }
            ImGuiFileDialog::Instance()->Close();
            m_ShowDirectorySelector = false;
        }
    }

    void PopupRenderMatrix::ResetToDefaults()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        const auto& layerStack = m_Context->GetProject()->GetLayerStack();
        m_LayerToRenderGroup.clear();

        for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
        {
            const auto& layer = layerStack.GetLayer(i);
            int currentGroup = static_cast<int>(layer.GetRenderGroup());
            m_LayerToRenderGroup[i] = currentGroup;
        }

        LUMINA_LOG_INFO("PopupRenderMatrix::ResetToDefaults: Reset to layer's current render groups");
    }

    void PopupRenderMatrix::ApplyRenderGroupChanges()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        auto& layerStack = m_Context->GetProject()->GetLayerStack();
        bool hasChanges = false;

        for (const auto& [layerIdx, renderGroup] : m_LayerToRenderGroup)
        {
            if (layerIdx < layerStack.GetLayerCount())
            {
                auto& layer = layerStack.GetLayer(layerIdx);
                RenderGroup newGroup = static_cast<RenderGroup>(renderGroup);

                if (layer.GetRenderGroup() != newGroup)
                {
                    layer.SetRenderGroup(newGroup);
                    hasChanges = true;
                    LUMINA_LOG_INFO("PopupRenderMatrix::ApplyRenderGroupChanges: Set layer '{}' to render group {}",
                        layer.GetName(), renderGroup);
                }
            }
        }

        if (hasChanges)
        {
            m_Context->GetProject()->MarkAsModified();
            LUMINA_LOG_INFO("PopupRenderMatrix::ApplyRenderGroupChanges: Applied render group changes to project");
        }
    }

    void PopupRenderMatrix::ExecuteExport()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        auto usedGroups = GetUsedRenderGroups();

        if (usedGroups.empty())
            return;

        if (usedGroups.size() == 1)
        {
            std::vector<size_t> layerIndices;
            const auto& layerStack = m_Context->GetProject()->GetLayerStack();

            for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
            {
                const auto& layer = layerStack.GetLayer(i);
                bool shouldExport = (m_ExportVisible && layer.GetVisibility()) ||
                    (m_ExportInvisible && !layer.GetVisibility());

                if (shouldExport && m_LayerToRenderGroup[i] == std::stoi(usedGroups[0]))
                {
                    layerIndices.push_back(i);
                }
            }

            if (!layerIndices.empty())
            {
                ExportRenderGroup(layerIndices, GetFullExportPath(GetExportFileName()));
            }
        }
        else
        {
            for (size_t groupIdx = 0; groupIdx < usedGroups.size(); ++groupIdx)
            {
                std::vector<size_t> layerIndices;
                const auto& layerStack = m_Context->GetProject()->GetLayerStack();
                int renderGroup = std::stoi(usedGroups[groupIdx]);

                for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
                {
                    const auto& layer = layerStack.GetLayer(i);
                    bool shouldExport = (m_ExportVisible && layer.GetVisibility()) ||
                        (m_ExportInvisible && !layer.GetVisibility());

                    if (shouldExport && m_LayerToRenderGroup[i] == renderGroup)
                    {
                        layerIndices.push_back(i);
                    }
                }

                if (!layerIndices.empty())
                {
                    ExportRenderGroup(layerIndices, GetFullExportPath(GetExportFileName(renderGroup)));
                }
            }
        }

        Hide();
    }

    std::vector<std::string> PopupRenderMatrix::GetUsedRenderGroups() const
    {
        std::set<int> usedGroups;

        for (const auto& pair : m_LayerToRenderGroup)
        {
            if (pair.second != -1) // Don't export disabled groups
            {
                usedGroups.insert(pair.second);
            }
        }

        std::vector<std::string> result;
        for (int group : usedGroups)
        {
            result.push_back(std::to_string(group));
        }

        return result;
    }

    std::string PopupRenderMatrix::GetExportFileName(int groupIndex) const
    {
        std::string baseName = m_ExportFileName;
        std::string extension = GetFormatExtension();

        if (groupIndex >= 0)
        {
            return baseName + "-" + std::to_string(groupIndex + 1) + extension;
        }

        return baseName + extension;
    }

    std::filesystem::path PopupRenderMatrix::GetFullExportPath(const std::string& fileName) const
    {
        return m_ExportDirectory / fileName;
    }

    const char* PopupRenderMatrix::GetFormatExtension() const
    {
        switch (m_ExportFormat)
        {
        case ExportFormat::PNG: return ".png";
        case ExportFormat::JPG: return ".jpg";
        case ExportFormat::BMP: return ".bmp";
        case ExportFormat::TGA: return ".tga";
        default: return ".png";
        }
    }

    void PopupRenderMatrix::ExportRenderGroup(const std::vector<size_t>& layerIndices, const std::filesystem::path& fileName)
    {
        if (layerIndices.empty() || !m_Context || !m_Context->HasProject())
            return;

        const auto& layerStack = m_Context->GetProject()->GetLayerStack();

        uint32_t width = layerStack.GetWidth() * Viewport::Render::DefaultTileSize;
        uint32_t height = layerStack.GetHeight() * Viewport::Render::DefaultTileSize;

        auto renderTarget = Renderer2D::CreateRenderTarget(width, height);
        auto camera = CreateRef<OrthographicCamera>();

        camera->SetPosition({
            width * 0.5f,
            height * 0.5f,
            1.0f
            });
        camera->SetSize(width, height);
        camera->SetZoom(1.0f);

        Renderer2D::SetRenderTarget(renderTarget);
        Renderer2D::SetResolution(width, height);
        Renderer2D::Begin(camera);

        glm::vec3 cameraPos = camera->GetPosition();
        const auto& textureAtlases = m_Context->GetProject()->GetTextureAtlases();
        float tileSize = Viewport::Render::DefaultTileSize;

        for (size_t layerIdx : layerIndices)
        {
            if (layerIdx >= layerStack.GetLayerCount())
                continue;

            const auto& layer = layerStack.GetLayer(layerIdx);

            for (size_t y = 0; y < layer.GetHeight(); ++y)
            {
                for (size_t x = 0; x < layer.GetWidth(); ++x)
                {
                    const Tile& tile = layer.GetTile(x, y);
                    if (!tile.IsPainted()) continue;

                    glm::vec2 tileWorldPos = {
                        (x + 1) * tileSize + cameraPos.x,
                        (y + 1) * tileSize + cameraPos.y
                    };

                    Renderer2D::SetQuadPosition({
                        tileWorldPos.x,
                        tileWorldPos.y,
                        layerIdx * 0.01f
                        });
                    Renderer2D::SetQuadRotation(tile.GetRotation());
                    Renderer2D::SetQuadTintColor(tile.GetTint());

                    glm::vec2 tileSizeMultiplier = tile.GetSize();
                    Renderer2D::SetQuadSize({
                        tileSize * tileSizeMultiplier.x,
                        tileSize * tileSizeMultiplier.y
                        });

                    if (tile.IsTextured() && tile.GetAtlasIndex() < textureAtlases.size())
                    {
                        auto atlas = textureAtlases[tile.GetAtlasIndex()];
                        if (atlas && atlas->HasTexture())
                        {
                            Renderer2D::SetQuadTexture(atlas->GetTexture());
                            Renderer2D::SetQuadTextureCoords(tile.GetTextureCoords());
                        }
                    }
                    else
                    {
                        Renderer2D::SetQuadTexture(nullptr);
                    }

                    Renderer2D::DrawQuad();
                }
            }
        }

        Renderer2D::End();
        Renderer2D::SetRenderTarget(nullptr);

        renderTarget->SaveToFile(fileName.string());
    }
}