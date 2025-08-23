#include "PopupRenderMatrix.h"
#include "Panels/Constants.h"
#include "Lumina/Lumina.h"
#include "ImGuiFileDialog.h"
#include <algorithm>
#include <sstream>
#include <set>

using namespace Lumina;

namespace Tiles
{
    PopupRenderMatrix::PopupRenderMatrix()
    {
    }

    void PopupRenderMatrix::Show(Ref<Context> context)
    {
        m_Context = context;
        m_IsVisible = true;

        if (m_Context && m_Context->HasProject())
        {
            const auto& project = m_Context->GetProject();
            std::string projectName = project->GetProjectName();
            std::transform(projectName.begin(), projectName.end(), projectName.begin(), ::tolower);
            std::replace(projectName.begin(), projectName.end(), ' ', '_');

            strncpy(m_ExportFileName, projectName.c_str(), sizeof(m_ExportFileName) - 1);
            m_ExportFileName[sizeof(m_ExportFileName) - 1] = '\0';

            ResetToDefaults();
        }
    }

    void PopupRenderMatrix::Render()
    {
        if (!m_IsVisible || !m_Context || !m_Context->HasProject())
            return;

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Export Render Matrix", &m_IsVisible, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize))
        {
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
    }

    void PopupRenderMatrix::RenderLayerMatrix()
    {
        if (!m_Context || !m_Context->HasProject())
            return;

        const auto& layerStack = m_Context->GetProject()->GetLayerStack();
        size_t layerCount = layerStack.GetLayerCount();

        ImGui::Text("Assign Layers to Render Groups (0-7)");
        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(UI::Component::SpaceBetween / 2.0f, 0.0f));
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

        float cell_width = 0.0f;
        float item_width = 0.0f;
        float cursor_x = 0.0f; 

        if (ImGui::BeginTable("RenderMatrix", RENDER_GROUPS + 2, tableFlags))
        {
            // Define Columns
            ImGui::TableSetupColumn("LayerName", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("LayerVisibility", ImGuiTableColumnFlags_WidthFixed, 25.0f);

            for (int group = 0; group < RENDER_GROUPS; ++group)
            {
                ImGui::TableSetupColumn(("RenderGroup" + std::to_string(group)).c_str(), ImGuiTableColumnFlags_WidthStretch);
            }

            ImGui::TableNextRow();

            // Column Headers 
            ImGui::TableSetColumnIndex(0);
            {
                const char* header = "Layer Name";
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header); 
            }

            ImGui::TableSetColumnIndex(1);
            {
                const char* header = "Vis";
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header);
            }

            for (int group = 0; group < RENDER_GROUPS; ++group)
            {
                ImGui::TableSetColumnIndex(group + 2);
                std::string header = std::to_string(group);

                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(header.c_str()).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f - ImGui::GetStyle().CellPadding.x;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::TableHeader(header.c_str());
            }

            // Rows
            for (size_t layerIdx = 0; layerIdx < layerCount; ++layerIdx)
            {
                const auto& layer = layerStack.GetLayer(layerIdx);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::CalcTextSize(layer.GetName().c_str()).x;
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                ImGui::SetCursorPosX(cursor_x);
                ImGui::Text("%s", layer.GetName().c_str());

                ImGui::TableSetColumnIndex(1);
                cell_width = ImGui::GetColumnWidth();
                item_width = ImGui::GetFrameHeight();
                cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                ImGui::SetCursorPosX(cursor_x);

                bool visible = layer.GetVisibility();
                ImGui::BeginDisabled(true);
                ImGui::Checkbox(("##vis" + std::to_string(layerIdx)).c_str(), &visible);
                ImGui::EndDisabled();

                for (int group = 0; group < RENDER_GROUPS; ++group)
                {
                    ImGui::TableSetColumnIndex(2 + group);

                    cell_width = ImGui::GetColumnWidth();
                    item_width = ImGui::GetFrameHeight();
                    cursor_x = ImGui::GetCursorPosX() + (cell_width - item_width) * 0.5f;
                    ImGui::SetCursorPosX(cursor_x);

                    bool isInGroup = m_LayerToRenderGroup[layerIdx] == group;
                    if (ImGui::RadioButton(("##" + std::to_string(layerIdx) + "_" + std::to_string(group)).c_str(), isInGroup))
                    {
                        m_LayerToRenderGroup[layerIdx] = group;
                    }
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
#if 0 

        float availableWidth = ImGui::GetContentRegionAvail().x - 200.0f;
        float groupColumnWidth = availableWidth / RENDER_GROUPS;
        float tableHeight = (layerCount + 1) * ImGui::GetTextLineHeightWithSpacing();

        if (ImGui::BeginTable("RenderMatrix", RENDER_GROUPS + 2,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(0.0f, tableHeight)))
        {
            ImGui::TableSetupColumn("Layer Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Vis", ImGuiTableColumnFlags_WidthFixed, 25.0f);

            for (int group = 0; group < RENDER_GROUPS; ++group)
            {
                ImGui::TableSetupColumn(std::to_string(group).c_str(),
                    ImGuiTableColumnFlags_WidthFixed, groupColumnWidth);
            }
            ImGui::TableHeadersRow();

            for (size_t layerIdx = 0; layerIdx < layerCount; ++layerIdx)
            {
                const auto& layer = layerStack.GetLayer(layerIdx);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", layer.GetName().c_str());

                ImGui::TableSetColumnIndex(1);
                bool visible = layer.GetVisibility();
                ImGui::BeginDisabled(true);
                ImGui::Checkbox(("##vis" + std::to_string(layerIdx)).c_str(), &visible);
                ImGui::EndDisabled();

                for (int group = 0; group < RENDER_GROUPS; ++group)
                {
                    ImGui::TableSetColumnIndex(2 + group);

                    bool isInGroup = m_LayerToRenderGroup[layerIdx] == group;
                    if (ImGui::RadioButton(("##" + std::to_string(layerIdx) + "_" + std::to_string(group)).c_str(), isInGroup))
                    {
                        m_LayerToRenderGroup[layerIdx] = group;
                    }
                }
            }
            ImGui::EndTable();
        }
#endif
    }

    void PopupRenderMatrix::RenderExportSettings()
    {
        ImGui::Text("Export Settings");
        ImGui::Spacing();

        ImGui::Text("Export Directory:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400.0f);
        ImGui::InputText("##ExportDir", m_ExportDirectory.data(), m_ExportDirectory.capacity(), ImGuiInputTextFlags_ReadOnly);
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
        ImGui::InputText("##FileName", m_ExportFileName, sizeof(m_ExportFileName));

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
        float totalWidth = buttonWidth * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        if (ImGui::Button("Export", ImVec2(buttonWidth, 0)))
        {
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
            Close();
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
            m_LayerToRenderGroup[i] = 0;
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

        Close();
    }

    std::vector<std::string> PopupRenderMatrix::GetUsedRenderGroups() const
    {
        std::set<int> usedGroups;

        for (const auto& pair : m_LayerToRenderGroup)
        {
            usedGroups.insert(pair.second);
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

    std::string PopupRenderMatrix::GetFullExportPath(const std::string& fileName) const
    {
        std::string directory = m_ExportDirectory;
        if (!directory.empty() && directory.back() != '/' && directory.back() != '\\')
        {
            directory += "/";
        }
        return directory + fileName;
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

    void PopupRenderMatrix::ExportRenderGroup(const std::vector<size_t>& layerIndices, const std::string& fileName)
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

        renderTarget->SaveToFile(fileName);
    }
}