// TextureSelectionPanel.h
#pragma once

#include "Lumina/Renderer/TextureAtlas.h"
#include "Lumina/Base.h"

#include <string>

#include "imgui.h"

class TextureSelectionPanel 
{
public:
    void Render();
    void Reset();

    // Accessors
    int GetSelectedTexture() const { return m_SelectedTexture; }
    void SetTextureAtlas(Lumina::Ref<Lumina::TextureAtlas> atlas) { m_TextureAtlas = atlas; }
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
    Lumina::Ref<Lumina::TextureAtlas> m_TextureAtlas;

    std::string m_TextureAtlasPath;
    int m_TextureAtlasWidth = 16;
    int m_TextureAtlasHeight = 16;
    int m_SelectedTexture = -1;
    bool m_IsAtlasLoaded = false;

    // Constants
    static constexpr float TEXTURE_BUTTON_SIZE = 40.0f;
    static constexpr float CHECKERBOARD_SIZE = 10.0f;
    static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255); // Orange
    static constexpr ImU32 DEAULT_BORDER_COLOR = IM_COL32(255, 255, 255, 255);  // White
    static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255); // Dark gray
	static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);    // Even darker gray
};