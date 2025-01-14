#pragma once

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

struct TileData
{
    bool UseTexture = false;
    size_t TextureIndex = -1;
};

struct LayerData
{
    std::string Name;
    std::vector<std::vector<TileData>> Layer;
};

struct TileAction
{
    size_t L = 0;
    size_t X = 0;
    size_t Y = 0;

    TileData Prev;
    TileData Curr;
};

struct HoveredTile
{
    size_t L = 0;
    size_t Y = 0;
    size_t X = 0;
};

class TileLayer
{
public:
    void Create(size_t width, size_t height);

    void AddLayer(std::string& name);
    void DeleteLayer();
    void ClearLayer();

    TileData& GetTile(size_t layer, size_t y, size_t x);
    void ResetTile(size_t y, size_t x);

    void FillLayer(size_t newTextureIndex, size_t y, size_t x);

    void ToggleLayerVisibilty(size_t layer);
    bool IsLayerVisible(size_t layer);

    size_t GetActiveLayer() const { return m_ActiveLayer; }
    void SetActiveLayer(size_t layer);

    const char* GetLayerName(size_t layer);
    void SetLayerName(std::string& name);

    void SetHoveredTile(size_t y, size_t x);
    void ResetHoveredTile() { m_HoveredTile = { 0, 0, 0 }; };
    TileData& GetHoveredTile();

    void RecordAction(TileAction action);
    void UndoAction();
    void RedoAction();

    void Save(const std::string& filename) const;
    void Load(const std::string& filename);

    size_t LayerWidth() const { return m_LayerWidth; }
    size_t LayerHeight() const { return m_LayerHeight; }
    size_t LayerSize() const { return m_TileLayers.size(); }
private:
    bool IsTileInBounds(size_t layer, size_t y, size_t x) const;
private:
    size_t m_LayerWidth;
    size_t m_LayerHeight;
    size_t m_ActiveLayer;

    HoveredTile m_HoveredTile;

    std::vector<LayerData> m_TileLayers;
    std::vector<bool> m_LayersVisible;

    std::stack<TileAction> m_UndoStack;
    std::stack<TileAction> m_RedoStack;
};