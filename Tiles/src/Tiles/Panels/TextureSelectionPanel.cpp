// TextureSelectionPanel.cpp
#include "TextureSelectionPanel.h"

#include "../Core/Color.h"

#include "ImGuiFileDialog.h"

#include <filesystem>
#include <algorithm>

namespace Tiles
{

    void TextureSelectionPanel::OnUIRender()
    {
        ImGui::Begin("Texture Selection");

        RenderAtlasPathSection();
        RenderFileDialog();

        ImGui::Separator();
        RenderAtlasDimensionsSection();
        ImGui::Separator();

        ImGui::BeginChild("TextureSelection", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        RenderTextureGrid();
        ImGui::EndChild();

        ImGui::End();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI Render
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureSelectionPanel::RenderAtlasPathSection()
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Atlas:");
        ImGui::SameLine();

        if (!m_Atlas->HasTexture())
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("[ No file selected ]");
            ImGui::SameLine();

            if (ImGui::Button("Browse..."))
                OpenFileDialog();
        }
        else
        {
            ImGui::AlignTextToFramePadding();
            std::string path = m_Atlas->GetTexture()->GetPath();
            std::string filename = path.substr(path.find_last_of("/\\") + 1);
            ImGui::TextWrapped("[ %s ]", filename.c_str());
            ImGui::SameLine();

            if (ImGui::Button("Remove"))
                m_Atlas->RemoveTexture();
        }
    }

    void TextureSelectionPanel::RenderAtlasDimensionsSection()
    {
        ImGui::Text("Atlas Dimensions");
        ImGui::PushItemWidth(100.0f);

        int width = m_Atlas->GetWidth();
        if (ImGui::InputInt(("##" + std::string("Width:")).c_str(), &width))
        {
            m_Atlas->Resize(std::max(1, width), m_Atlas->GetHeight());
        }

        int height = m_Atlas->GetHeight();
        if (ImGui::InputInt(("##" + std::string("Height:")).c_str(), &height))
        {
            m_Atlas->Resize(m_Atlas->GetWidth(), std::max(1, height));
        }

        ImGui::PopItemWidth();
    }

    void TextureSelectionPanel::RenderTextureGrid()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        ImVec2 position = ImGui::GetCursorScreenPos();

        // Compute the full size based on tile size and grid count
        float atlasWidthPixels = m_Atlas->GetWidth() * m_TextureButtonSize;
        float atlasHeightPixels = m_Atlas->GetHeight() * m_TextureButtonSize;

        // Render checkerboard
        // TODO:: Use Shader to render background like in viewport.
        for (float y = position.y; y < position.y + atlasHeightPixels; y += m_CheckerboardSize)
        {
            for (float x = position.x; x < position.x + atlasWidthPixels; x += m_CheckerboardSize)
            {
                ImVec2 minPos = ImVec2(x, y);
                ImVec2 maxPos = ImVec2(x + m_CheckerboardSize, y + m_CheckerboardSize);

                if (((int)(x / m_CheckerboardSize) + (int)(y / m_CheckerboardSize)) % 2 == 0)
                    ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, Color::CHECKERBOARD_COLOR_1);
                else
                    ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, Color::CHECKERBOARD_COLOR_2);
            }
        }

        // Render texture grid on top of the checkerboard
        for (int y = 0; y < m_Atlas->GetHeight(); y++)
        {
            for (int x = 0; x < m_Atlas->GetWidth(); x++)
            {
                int index = y * m_Atlas->GetWidth() + x;
                RenderTextureGridItem(index, x, y);

                if ((index + 1) % m_Atlas->GetWidth() != 0)
                    ImGui::SameLine();
            }
        }

        ImGui::PopStyleVar(4);
    }

    void TextureSelectionPanel::RenderTextureGridItem(int index, int x, int y)
    {
        ImVec2 buttonSize(m_TextureButtonSize, m_TextureButtonSize);

        // Render texture or transparent empty button
        if (m_Atlas->HasTexture())
        {
            glm::vec4 texCoords = m_Atlas->GetTextureCoords(index);
            ImVec2 xy(texCoords.x, texCoords.y);
            ImVec2 zw(texCoords.z, texCoords.w);
            intptr_t textureID = (intptr_t)m_Atlas->GetTexture()->GetID();

            ImGui::Image((void*)textureID, buttonSize, xy, zw, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            ImGui::Button(("##" + std::to_string(x) + std::to_string(y)).c_str(), buttonSize);
            ImGui::PopStyleColor(3);
        }

        // Handle selection
        if (ImGui::IsItemClicked())
        {
            if (m_TextureSelection->IsSelected(index))
            {
                m_TextureSelection->Deselect(index);
            }
            else
            {
                m_TextureSelection->Select(index);
                m_ToolSelection->Erase = false;
            }
        }

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        // Highlight selected texture or default grid
        if (m_TextureSelection->IsSelected(index))
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::SELECTION_BORDER_COLOR, 0.0f, 0, 2.5f);
        else
            ImGui::GetWindowDrawList()->AddRect(min, max, Color::DEAULT_BORDER_COLOR, 0.0f, 0, 0.0f);
    }

    void TextureSelectionPanel::RenderFileDialog()
    {
        if (ImGuiFileDialog::Instance()->Display("ChooseAtlasFileDlg",
            ImGuiWindowFlags_NoCollapse,
            ImVec2(600, 400)))
        {
            HandleFileDialogResult();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // Helper Methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureSelectionPanel::HandleAtlasFileSelection(const std::string& newPath)
    {
        if (std::filesystem::exists(newPath))
        {
            std::filesystem::path relativePath = std::filesystem::relative(newPath, std::filesystem::current_path());
            m_Atlas->SetTexture(relativePath.string());
        }
    }

    void TextureSelectionPanel::OpenFileDialog()
    {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.flags = ImGuiFileDialogFlags_Modal;
        config.countSelectionMax = 1;

        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseAtlasFileDlg",
            "Choose Atlas File",
            ".png,.jpg,.jpeg",
            config
        );
    }

    void TextureSelectionPanel::HandleFileDialogResult()
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            HandleAtlasFileSelection(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

}