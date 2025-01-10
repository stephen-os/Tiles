#pragma once

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

struct TileData
{
    bool UseTexture = false;
    uint32_t TextureIndex = 0;
};

struct LayerData
{
    std::string Name;
    std::vector<std::vector<TileData>> Layer;
};

struct TileAction
{
    uint32_t L;
    uint32_t X;
    uint32_t Y;

    TileData Prev;
    TileData Curr; 
};

class TileLayer
{
public:
    TileLayer();
    ~TileLayer() = default; 

    void Init(uint32_t width, uint32_t height);

    LayerData& GetLayer(uint32_t index) { return m_Layers[index]; }
    void AddLayer(std::string& name);
    void DeleteLayer(uint32_t index);
    void ClearLayer(uint32_t index);

    TileData& GetTileData(uint32_t layer, uint32_t x, uint32_t y);
    void ClearTile(uint32_t layer, uint32_t x, uint32_t y);

    void FillLayer(uint32_t newTextureIndex, uint32_t layer, uint32_t x, uint32_t y);

    void RecordAction(TileAction action);
    void UndoAction();
    void RedoAction();

    void SaveLayers(const std::string & filename);
    void LoadLayers(const std::string& filename);

    uint32_t GetWidth() { return m_LayerWidth; }
    uint32_t GetHeight() { return m_LayerHeight; }
    uint32_t Size() { return m_Layers.size(); }
private:
    uint32_t m_LayerWidth;
    uint32_t m_LayerHeight; 
    std::vector<LayerData> m_Layers; 

    std::stack<TileAction> m_UndoStack;
    std::stack<TileAction> m_RedoStack;
};