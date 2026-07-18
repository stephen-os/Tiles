#pragma once

#include "Panel.h"

#include "Tiles.h"

namespace Tiles::Editor
{
    // Panel for the project's texture atlases: a collapsible section per atlas
    // (rename, source image, grid dimensions, remove) whose cells pick the texture
    // applied to the current brush. Atlases are referenced by stable id, so the
    // picked tile survives atlas add / remove / reorder.
    class PanelTextureSelection : public Panel
    {
    public:
        PanelTextureSelection(EditorHost& host);
        ~PanelTextureSelection() = default;

        void Render() override;
        void Update() override;

    private:
        void RenderBlockToolbar();
        void RenderBlockAtlasList();
        void RenderBlockFileDialog();

        // One collapsible section for the atlas at atlasIndex. Returns true if its
        // "Remove Atlas" was clicked this frame; removal is deferred to after the
        // list loop so the atlas vector is not mutated mid-iteration.
        bool RenderComponentAtlasHeader(size_t atlasIndex);

        void RenderSectionAtlasName(size_t atlasIndex);
        void RenderSectionAtlasImage(size_t atlasIndex);
        void RenderSectionAtlasDimensions(size_t atlasIndex);
        void RenderSectionTextureGrid(size_t atlasIndex, float tileSize);

        void RenderComponentTextureGridItem(const char* id, int index, size_t atlasIndex, float tileSize);
        void RenderComponentSelectionBorder(int index, size_t atlasIndex, float tileSize);
        void RenderComponentFilePathDisplay(const std::string& path);

        // Chooses a grid cell size that fits atlasWidth cells across availableWidth,
        // clamped to min/max bounds and biased toward larger cells for small atlases.
        float CalculateDynamicTileSize(float availableWidth, int atlasWidth) const;

        // The label shown for an atlas: its name, or a positional "Atlas N" fallback.
        std::string AtlasDisplayName(const Tiles::TextureAtlas& atlas, size_t atlasIndex) const;

        // Points the atlas awaiting a file-dialog result at newPath, stored relative
        // to the working directory so saved projects stay portable. No-op if the
        // file is missing or that atlas is gone.
        void HandleAtlasFileSelection(const std::string& newPath);

        // Applies the clicked tile to the brush, or clears the brush's texture if
        // that same tile is already selected (click-to-toggle).
        void HandleTextureSelection(int index, size_t atlasIndex);

        // Commits the drag rectangle: a single cell sets the click-to-toggle brush;
        // a larger rectangle becomes a multi-cell stamp.
        void FinalizeSelection(size_t atlasIndex);

        void OpenFileDialog(size_t atlasIndex);
        void HandleFileDialogResult();
        void AddNewAtlas();

    private:
        std::shared_ptr<Tiles::Texture> m_CheckerboardTexture = nullptr;
        AtlasId m_DialogAtlasId = AtlasId::Invalid;   // atlas awaiting a Browse... result

        // Drag-to-select state, in atlas grid (col, row). The anchor/current pair
        // doubles as the committed selection rectangle for highlighting.
        bool m_Selecting = false;
        bool m_HasStampSelection = false;
        AtlasId m_SelectAtlasId = AtlasId::Invalid;
        glm::ivec2 m_SelectAnchor = { 0, 0 };
        glm::ivec2 m_SelectCurrent = { 0, 0 };
    };
}
