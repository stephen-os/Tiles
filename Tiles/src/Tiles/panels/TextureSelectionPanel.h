// TextureSelectionPanel.h
#pragma once

#include <string>
#include "Lumina/Renderer/TextureAtlas.h"
#include "imgui.h"

class TextureSelectionPanel {
public:
    void Render();
    void Reset();

    // Accessors
    int GetSelectedTexture() const { return m_SelectedTexture; }
    Lumina::TextureAtlas& GetTextureAtlas() { return m_TextureAtlas; }
    bool IsAtlasLoaded() const { return m_IsAtlasLoaded; }

private:
    // UI Rendering Methods
    void RenderAtlasPathSection();
    void RenderFileDialog();
    void RenderAtlasDimensionsSection();
    void RenderTextureGrid();

    // Helper Methods
    void HandleAtlasFileSelection(const std::string& newPath);
    void LoadTextureAtlas();
    void UpdateAtlasDimensions(int& dimension, const char* label);
    void RenderTextureGridItem(int index, int x, int y);

    // File Dialog Methods
    void OpenFileDialog();
    void HandleFileDialogResult();

private:
    // Texture Atlas State
    Lumina::TextureAtlas m_TextureAtlas;
    std::string m_TextureAtlasPath;
    int m_TextureAtlasWidth = 16;
    int m_TextureAtlasHeight = 16;
    int m_SelectedTexture = -1;
    bool m_IsAtlasLoaded = false;

    // Constants
    static constexpr float TEXTURE_BUTTON_SIZE = 40.0f;
    static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(169, 169, 169, 255);
};