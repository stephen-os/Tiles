// TextureSelectionPanel.cpp
#include "TextureSelectionPanel.h"

#include "ImGuiFileDialog.h"

#include <filesystem>
#include <algorithm>

void TextureSelectionPanel::Render() 
{
    ImGui::Begin("Texture Selection");

    RenderAtlasPathSection();
    RenderFileDialog();

    if (m_IsAtlasLoaded) 
    {
        RenderAtlasDimensionsSection();
        RenderTextureGrid();
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
    config.sidePaneWidth = 250.0f;

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
        LoadTextureAtlas();
    }
}

void TextureSelectionPanel::LoadTextureAtlas() 
{
    try 
    {
        m_TextureAtlas->CreateAtlas(m_TextureAtlasPath, m_TextureAtlasWidth, m_TextureAtlasHeight);
        m_IsAtlasLoaded = true;
    }
    catch (...) 
    {
        m_IsAtlasLoaded = false;
        // You might want to add error handling/logging here
    }
}

void TextureSelectionPanel::RenderAtlasDimensionsSection() 
{
    ImGui::Text("Atlas Dimensions");
    ImGui::PushItemWidth(100.0f);

    UpdateAtlasDimensions(m_TextureAtlasWidth, "Width:");
    UpdateAtlasDimensions(m_TextureAtlasHeight, "Height:");

    ImGui::PopItemWidth();
}

void TextureSelectionPanel::UpdateAtlasDimensions(int& dimension, const char* label) {
    ImGui::Text(label);

    int tempDimension = dimension;
    if (ImGui::InputInt(("##" + std::string(label)).c_str(), &tempDimension)) 
    {
        dimension = std::max(1, tempDimension);
        if (m_IsAtlasLoaded) 
        {
            m_TextureAtlas->CalculateTexCoords(m_TextureAtlasWidth, m_TextureAtlasHeight);
        }
    }
}

void TextureSelectionPanel::RenderTextureGrid()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    ImVec2 position = ImGui::GetCursorScreenPos();
    
    // Compute the full size based on tile size and grid count
    float atlasWidthPixels = m_TextureAtlas->GetGridWidth() * TEXTURE_BUTTON_SIZE;
    float atlasHeightPixels = m_TextureAtlas->GetGridHeight() * TEXTURE_BUTTON_SIZE;

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
    for (int y = 0; y < m_TextureAtlas->GetGridHeight(); ++y)
    {
        for (int x = 0; x < m_TextureAtlas->GetGridWidth(); ++x)
        {
            int index = y * m_TextureAtlas->GetGridWidth() + x;
            RenderTextureGridItem(index, x, y);

            if ((index + 1) % m_TextureAtlas->GetGridWidth() != 0)
                ImGui::SameLine();
        }
    }

    ImGui::PopStyleVar(4);
}

void TextureSelectionPanel::RenderTextureGridItem(int index, int x, int y)
{
    glm::vec4 texCoords = m_TextureAtlas->GetTexCoords(index);
    ImVec2 buttonSize(TEXTURE_BUTTON_SIZE, TEXTURE_BUTTON_SIZE);
    ImVec2 xy(texCoords.x, texCoords.y);
    ImVec2 zw(texCoords.z, texCoords.w);
    intptr_t textureID = (intptr_t)m_TextureAtlas->GetTextureID();

    // Render texture with full transparency support
    ImGui::Image((void*)textureID, buttonSize, xy, zw, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));

    // Handle selection
    if (ImGui::IsItemClicked())
        m_SelectedTexture = (m_SelectedTexture == index) ? -1 : index;

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();

    // Highlight selected texture or default grid
    if (index == m_SelectedTexture)
        ImGui::GetWindowDrawList()->AddRect(min, max, SELECTION_BORDER_COLOR, 0.0f, 0, 2.5f);
    else
        ImGui::GetWindowDrawList()->AddRect(min, max, DEAULT_BORDER_COLOR, 0.0f, 0, 1.0f);
}


void TextureSelectionPanel::Reset() 
{
    m_SelectedTexture = -1;
    m_TextureAtlasPath.clear();
    m_TextureAtlasWidth = 16;
    m_TextureAtlasHeight = 16;
    m_IsAtlasLoaded = false;
}