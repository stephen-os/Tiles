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

// TODO: Texture atlas should have an update function that just updates the offsets
void TextureSelectionPanel::LoadTextureAtlas() 
{
    try 
    {
        m_TextureAtlas.CreateAtlas(m_TextureAtlasPath, m_TextureAtlasWidth, m_TextureAtlasHeight);
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
            LoadTextureAtlas();
        }
    }
}

void TextureSelectionPanel::RenderTextureGrid() 
{
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("TextureSelectionChild", availableSize, true,
        ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    for (int y = 0; y < m_TextureAtlas.GetGridHeight(); ++y) 
    {
        for (int x = 0; x < m_TextureAtlas.GetGridWidth(); ++x) 
        {
            int index = y * m_TextureAtlas.GetGridWidth() + x;
            RenderTextureGridItem(index, x, y);

            if ((index + 1) % m_TextureAtlas.GetGridWidth() != 0) 
                ImGui::SameLine();
        }
    }

    ImGui::EndChild();
}

void TextureSelectionPanel::RenderTextureGridItem(int index, int x, int y) 
{
    glm::vec4 texCoords = m_TextureAtlas.GetTexCoords(index);
    ImVec2 buttonSize(TEXTURE_BUTTON_SIZE, TEXTURE_BUTTON_SIZE);
    ImVec2 xy(texCoords.x, texCoords.y);
    ImVec2 zw(texCoords.z, texCoords.w);
    intptr_t textureID = (intptr_t)m_TextureAtlas.GetTextureID();

    ImGui::ImageButton((void*)textureID, buttonSize, xy, zw);

    if (ImGui::IsItemClicked()) 
        m_SelectedTexture = (m_SelectedTexture == index) ? -1 : index;

    if (index == m_SelectedTexture) {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(min, max, SELECTION_BORDER_COLOR, 3.0f, 0, 1.5f);
    }
}

void TextureSelectionPanel::Reset() 
{
    m_SelectedTexture = -1;
    m_TextureAtlasPath.clear();
    m_TextureAtlasWidth = 16;
    m_TextureAtlasHeight = 16;
    m_IsAtlasLoaded = false;
}