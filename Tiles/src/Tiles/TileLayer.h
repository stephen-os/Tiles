#pragma once

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

struct TileData
{
    glm::vec4 Color = {0.3f, 0.3f, 0.3f, 1.0f};
    float Opacity = 1.0f;

    bool UseTexture = false;
    int TextureIndex = 0;
};

struct LayerData
{
    std::string Name;
    std::vector<std::vector<TileData>> Layer;
};

struct TileAction
{
    uint32_t Index;
    uint32_t X;
    uint32_t Y;

    TileData Tile;
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

    void RecordAction(uint32_t index, uint32_t x, uint32_t y);
    void UndoAction();
    void RedoAction();

    void SaveLayers(const std::string & filename);
    void LoadLayers(const std::string& filename);

    uint32_t GetWidth() { return m_LayerWidth; }
    uint32_t GetHeight() { return m_LayerHeight; }
    uint32_t Size() { return m_Layers.size(); }

    using iterator = std::vector<LayerData>::iterator;
    using const_iterator = std::vector<LayerData>::const_iterator;

    iterator begin() { return m_Layers.begin(); }
    iterator end() { return m_Layers.end(); }
    const_iterator begin() const { return m_Layers.begin(); }
    const_iterator end() const { return m_Layers.end(); }
private:
    uint32_t m_LayerWidth;
    uint32_t m_LayerHeight; 
    std::vector<LayerData> m_Layers; 

    std::stack<TileAction> m_UndoStack;
    std::stack<TileAction> m_RedoStack;
};