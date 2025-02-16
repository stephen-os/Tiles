// TextureSelectionPanel.cpp
#include "TextureSelectionPanel.h"

#include "ImGuiFileDialog.h"

#include <filesystem>
#include <algorithm>

namespace Tiles
{

    void TextureSelectionPanel::Render()
    {
        ImGui::Begin("Texture Selection");

        RenderAtlasPathSection();
        RenderFileDialog();

        if (m_Atlas->IsCreated())
        {
            RenderAtlasDimensionsSection();
            ImGui::Separator();

            ImGui::BeginChild("TextureSelection", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            RenderTextureGrid();
            ImGui::EndChild();
        }

        ImGui::End();
    }

    void TextureSelectionPanel::RenderAtlasPathSection()
    {
        ImGui::Text("Atlas Path: ");
        ImGui::SameLine();
        ImGui::Text(m_TextureAtlasPath.empty() ? "No file selected" : m_TextureAtlasPath.c_str());
        ImGui::SameLine();

        if (ImGui::Button("Browse...")) {
            OpenFileDialog();
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

    void TextureSelectionPanel::RenderFileDialog()
    {
        if (ImGuiFileDialog::Instance()->Display("ChooseAtlasFileDlg",
            ImGuiWindowFlags_NoCollapse,
            ImVec2(600, 400)))
        {
            HandleFileDialogResult();
        }
    }

    void TextureSelectionPanel::HandleAtlasFileSelection(const std::string& newPath)
    {
        if (std::filesystem::exists(newPath))
        {
            m_TextureAtlasPath = newPath;
            m_Atlas->Create(m_TextureAtlasPath);
        }
    }

    void TextureSelectionPanel::RenderAtlasDimensionsSection()
    {
        ImGui::Text("Atlas Dimensions");
        ImGui::PushItemWidth(100.0f);

        if (ImGui::InputInt(("##" + std::string("Width:")).c_str(), &m_Atlas->GetWidth()))
        {
            m_Atlas->GetWidth() = std::max(1, m_Atlas->GetWidth());
            if (m_Atlas->IsCreated())
            {
                m_Atlas->UpdateTexCoords();
            }
        }

        if (ImGui::InputInt(("##" + std::string("Height:")).c_str(), &m_Atlas->GetHeight()))
        {
            m_Atlas->GetWidth() = std::max(1, m_Atlas->GetWidth());
            if (m_Atlas->IsCreated())
            {
                m_Atlas->UpdateTexCoords();
            }
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
        float atlasWidthPixels = m_Atlas->GetWidth() * TEXTURE_BUTTON_SIZE;
        float atlasHeightPixels = m_Atlas->GetHeight() * TEXTURE_BUTTON_SIZE;

        // Render checkerboard
        for (float y = position.y; y < position.y + atlasHeightPixels; y += CHECKERBOARD_SIZE)
        {
            for (float x = position.x; x < position.x + atlasWidthPixels; x += CHECKERBOARD_SIZE)
            {
                ImVec2 minPos = ImVec2(x, y);
                ImVec2 maxPos = ImVec2(x + CHECKERBOARD_SIZE, y + CHECKERBOARD_SIZE);

                ImU32 fillColor = (((int)(x / CHECKERBOARD_SIZE) + (int)(y / CHECKERBOARD_SIZE)) % 2 == 0) ? CHECKERBOARD_COLOR_1 : CHECKERBOARD_COLOR_2;
                ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, fillColor);
            }
        }

        // Render texture grid on top of the checkerboard
        for (int y = 0; y < m_Atlas->GetHeight(); ++y)
        {
            for (int x = 0; x < m_Atlas->GetWidth(); ++x)
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
        glm::vec4 texCoords = m_Atlas->GetTexCoords(index);
        ImVec2 buttonSize(TEXTURE_BUTTON_SIZE, TEXTURE_BUTTON_SIZE);
        ImVec2 xy(texCoords.x, texCoords.y);
        ImVec2 zw(texCoords.z, texCoords.w);
        intptr_t textureID = (intptr_t)m_Atlas->GetTextureID();

        // Render texture with full transparency support
        ImGui::Image((void*)textureID, buttonSize, xy, zw, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));

        // Handle selection
        if (ImGui::IsItemClicked())
        {
            if (index == m_Atlas->GetSelectedTexture())
                m_Atlas->DeselectTexture();
            else
                m_Atlas->SelectTexture(index);
        }

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        // Highlight selected texture or default grid
        if (index == m_Atlas->GetSelectedTexture())
            ImGui::GetWindowDrawList()->AddRect(min, max, SELECTION_BORDER_COLOR, 0.0f, 0, 2.5f);
        else
            ImGui::GetWindowDrawList()->AddRect(min, max, DEAULT_BORDER_COLOR, 0.0f, 0, 1.0f);
    }

}