#pragma once

#include "Panel.h"

#include "Lumina/Lumina.h"

namespace Tiles
{
    class PanelTextureSelection : public Panel
    {
    public:
        PanelTextureSelection(Ref<Context> context);
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

        float CalculateDynamicTileSize(float availableWidth, int atlasWidth) const;
        void HandleAtlasFileSelection(const std::string& newPath);
        void HandleTextureSelection(int index, size_t atlasIndex);
        void OpenFileDialog();
        void HandleFileDialogResult();
        void AddNewAtlas();
        void RemoveCurrentAtlas();
        void SetCurrentAtlasIndex(size_t index);
        bool HasValidCurrentAtlas() const;

    private:
        Lumina::Ref<Lumina::Texture> m_CheckerboardTexture = nullptr;
        size_t m_CurrentAtlasIndex = 0;
    };
}