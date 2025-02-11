#pragma once

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

struct TileData
{
    size_t TextureIndex = -1;
};

struct LayerData
{
    std::string Name;
    std::vector<std::vector<TileData>> Layer;
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

    void SetLastMousePosition(glm::vec3 position) { m_LastMousePosition = position; }
    glm::vec3 GetLastMousePosition() const { return m_LastMousePosition; }

    const char* GetLayerName(size_t layer);
    void SetLayerName(std::string& name);

    size_t LayerWidth() const { return m_LayerWidth; }
    size_t LayerHeight() const { return m_LayerHeight; }
    size_t LayerSize() const { return m_TileLayers.size(); }
private:
    bool IsTileInBounds(size_t layer, size_t y, size_t x) const;
private:
    size_t m_LayerWidth;
    size_t m_LayerHeight;
    size_t m_ActiveLayer;

    glm::vec3 m_LastMousePosition;

    std::vector<LayerData> m_TileLayers;
    std::vector<bool> m_LayersVisible;
};