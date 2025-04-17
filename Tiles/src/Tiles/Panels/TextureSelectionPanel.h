#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "../Core/TextureSelection.h"

#include "Lumina/Core/Aliases.h"

#include <string>

#include "imgui.h"

namespace Tiles
{

    class TextureSelectionPanel
    {
    public:
        TextureSelectionPanel() = default;
        ~TextureSelectionPanel() = default;

        void OnUIRender();

        // Setters
        void SetTextureAtlas(Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; }
        void SetTextureSelection(Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; }
    private:
        // UI Rendering Methods
        void RenderAtlasPathSection();
        void RenderAtlasDimensionsSection();
        void RenderTextureGrid();
        void RenderTextureGridItem(int index, int x, int y);
        void RenderFileDialog();

        // Helper Methods
        void HandleAtlasFileSelection(const std::string& newPath);
        void OpenFileDialog();
        void HandleFileDialogResult();

    private:
        Shared<Lumina::TextureAtlas> m_Atlas;
        Shared<TextureSelection> m_TextureSelection;

        float m_TextureButtonSize = 40.0f;
        float m_CheckerboardSize = 10.0f;
    };

}