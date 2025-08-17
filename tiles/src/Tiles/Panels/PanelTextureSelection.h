#pragma once

#include "Panel.h"

#include "Lumina/Lumina.h"

#include "imgui.h"

using namespace Lumina;

namespace Tiles
{
    class PanelTextureSelection : public Panel
    {
    public:
        PanelTextureSelection();
        ~PanelTextureSelection() = default;

        void Render() override;
		void Update() override;

    private:
        // Main UI sections
        void RenderAtlasTabs();
        void RenderAtlasControls();
        void RenderCurrentAtlasContent();
        void RenderFileDialog();

        // Atlas content sections
        void RenderAtlasPathSection();
        void RenderAtlasDimensionsSection();
        void RenderTextureGrid();

        // Texture grid helpers
        float CalculateDynamicTileSize(float availableWidth, int atlasWidth) const;
        void RenderTextureGridItem(int index, int x, int y, size_t atlasIndex, float tileSize);

        // File handling
        void HandleAtlasFileSelection(const std::string& newPath);
        void OpenFileDialog();
        void HandleFileDialogResult();

        // Atlas management
        void AddNewAtlas();
        void RemoveCurrentAtlas();
        void SetCurrentAtlasIndex(size_t index);
        bool HasValidCurrentAtlas() const;

        void HandleTextureSelection(int index, size_t atlasIndex);
        void DrawSelectionBorder(int index, size_t atlasIndex, float tileSize);
    private:
        size_t m_CurrentAtlasIndex = 0;
        Ref<Texture> m_CheckerBoard;
    };
}