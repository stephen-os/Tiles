#pragma once

#include "Panel.h"

#include "Tiles.h"

namespace Tiles::Editor
{
    // Panel for the project's texture atlases: a tab per atlas, controls to
    // add/remove atlases and set an atlas image and grid dimensions, and a tile
    // grid whose cells pick the texture applied to the current brush.
    class PanelTextureSelection : public Panel
    {
    public:
        PanelTextureSelection(EditorHost& host);
        ~PanelTextureSelection() = default;

        void Render() override;
        void Update() override;

    private:
        void RenderBlockAtlasTabs();
        void RenderBlockAtlasControls();
        void RenderBlockCurrentAtlasContent();
        void RenderBlockFileDialog();

        void RenderSectionAtlasPath();
        void RenderSectionAtlasDimensions();
        void RenderSectionTextureGrid();

        void RenderComponentAtlasTab(const char* id, size_t atlasIndex, const char* tabName);
        void RenderComponentTextureGridItem(const char* id, int index, int x, int y, size_t atlasIndex, float tileSize);
        void RenderComponentSelectionBorder(const char* id, int index, size_t atlasIndex, float tileSize);
        void RenderComponentFilePathDisplay(const char* id, const std::string& path);
        void RenderComponentDimensionInput(const char* id, const char* label, int* value);

        // Chooses a grid cell size that fits atlasWidth cells across availableWidth,
        // clamped to min/max bounds and biased toward larger cells for small atlases.
        float CalculateDynamicTileSize(float availableWidth, int atlasWidth) const;

        // Points the current atlas at newPath, stored relative to the working
        // directory so saved projects stay portable. No-op if the file is missing.
        void HandleAtlasFileSelection(const std::string& newPath);

        // Applies the clicked tile to the brush, or clears the brush's texture if
        // that same tile is already selected (click-to-toggle).
        void HandleTextureSelection(int index, size_t atlasIndex);
        void OpenFileDialog();
        void HandleFileDialogResult();
        void AddNewAtlas();
        void RemoveCurrentAtlas();
        void SetCurrentAtlasIndex(size_t index);
        bool HasValidCurrentAtlas() const;

    private:
        std::shared_ptr<Tiles::Texture> m_CheckerboardTexture = nullptr;
        size_t m_CurrentAtlasIndex = 0;
    };
}