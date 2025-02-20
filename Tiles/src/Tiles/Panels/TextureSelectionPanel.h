#pragma once

#include "../Core/Atlas.h"
#include "../Core/Selection.h"
#include "../Core/Base.h"

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
        void SetTextureAtlas(Shared<Atlas> atlas) { m_Atlas = atlas; }
        void SetSelection(Shared<Selection> selection) { m_Selection = selection; }
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
        Shared<Atlas> m_Atlas;
        Shared<Selection> m_Selection;

        float m_TextureButtonSize = 40.0f;
        float m_CheckerboardSize = 10.0f;
    };

}