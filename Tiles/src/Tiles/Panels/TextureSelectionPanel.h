// TextureSelectionPanel.h
#pragma once

#include "Lumina/Base.h"

#include "../Core/Atlas.h"

#include <string>

#include "imgui.h"

class TextureSelectionPanel 
{
public:
	TextureSelectionPanel() = default;
    ~TextureSelectionPanel() = default;

    void Render();
    void Reset();
    // Setters
    void SetTextureAtlas(Lumina::Ref<Atlas> atlas) { m_Atlas = atlas; }
private:
    // UI Rendering Methods
    void RenderAtlasPathSection();
    void RenderFileDialog();
    void RenderAtlasDimensionsSection();
    void RenderTextureGrid();

    // Helper Methods
    void HandleAtlasFileSelection(const std::string& newPath);
    void UpdateAtlasDimensions(int& dimension, const char* label);
    void RenderTextureGridItem(int index, int x, int y);

    // File Dialog Methods
    void OpenFileDialog();
    void HandleFileDialogResult();

private:
    // Texture Atlas State
    Lumina::Ref<Atlas> m_Atlas;

    std::string m_TextureAtlasPath;
    int m_TextureAtlasWidth = 16;
    int m_TextureAtlasHeight = 16;

    // Constants
    static constexpr float TEXTURE_BUTTON_SIZE = 40.0f;
    static constexpr float CHECKERBOARD_SIZE = 10.0f;
    static constexpr ImU32 SELECTION_BORDER_COLOR = IM_COL32(255, 165, 0, 255); // Orange
    static constexpr ImU32 DEAULT_BORDER_COLOR = IM_COL32(255, 255, 255, 255);  // White
    static constexpr ImU32 CHECKERBOARD_COLOR_1 = IM_COL32(120, 120, 120, 255); // Dark gray
	static constexpr ImU32 CHECKERBOARD_COLOR_2 = IM_COL32(80, 80, 80, 255);    // Even darker gray
};