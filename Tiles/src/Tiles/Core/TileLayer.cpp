#include "TileLayer.h"

#include "TileSerializer.h"

#include <stdexcept>
#include <queue>
#include <iostream>

void TileLayer::Create(size_t width, size_t height)
{
    m_LayerWidth = width;
    m_LayerHeight = height;

    m_ActiveLayer = 0;

    m_TileLayers.clear();
    m_LayersVisible.clear();

    ResetHoveredTile();

    while (!m_UndoStack.empty())
    {
        m_UndoStack.pop();
    }

    while (!m_RedoStack.empty())
    {
        m_RedoStack.pop();
    }

    std::string name = "Layer 1";
    AddLayer(name);
}

void TileLayer::AddLayer(std::string& name)
{
    LayerData layer;
    layer.Name = name;

    layer.Layer.resize(m_LayerHeight, std::vector<TileData>(m_LayerWidth));

    for (size_t y = 0; y < m_LayerHeight; y++)
    {
        for (size_t x = 0; x < m_LayerWidth; x++)
        {
            layer.Layer[y][x] = TileData();
        }
    }

    m_TileLayers.push_back(layer);

    m_LayersVisible.resize(LayerSize(), true);

    ResetHoveredTile();
}

void TileLayer::DeleteLayer()
{
    if (!m_TileLayers.empty())
    {
        if (LayerSize() == 1)
        {
            ClearLayer();
            m_TileLayers[0].Name = "Layer 1";
        }
        else
        {
            m_TileLayers.erase(m_TileLayers.begin() + m_ActiveLayer);
            m_ActiveLayer = (m_ActiveLayer > 0) ? m_ActiveLayer - 1 : 0;
        }
    }

    ResetHoveredTile();
}

void TileLayer::ClearLayer()
{
    auto& layer = m_TileLayers[m_ActiveLayer].Layer;
    for (auto& row : layer)
    {
        for (auto& tile : row)
        {
            tile = TileData();
        }
    }
}

TileData& TileLayer::GetTile(size_t layer, size_t y, size_t x)
{
    if (IsTileInBounds(layer, y, x))
    {
        return m_TileLayers[layer].Layer[y][x];
    }
    else
    {
        throw std::out_of_range("Tile is out of bounds.");
    }
}

void TileLayer::ResetTile(size_t y, size_t x)
{
    if (IsTileInBounds(m_ActiveLayer, y, x))
    {
        m_TileLayers[m_ActiveLayer].Layer[y][x] = TileData();
    }
    else
    {
        throw std::out_of_range("Tile is out of bounds.");
    }
}

void TileLayer::FillLayer(size_t newTextureIndex, size_t y, size_t x)
{
    if (!IsTileInBounds(m_ActiveLayer, y, x))
        return;

    size_t oldTextureIndex = m_TileLayers[m_ActiveLayer].Layer[y][x].TextureIndex;

    if (newTextureIndex == oldTextureIndex)
        return;

    std::queue<std::pair<size_t, size_t>> tileQueue;
    tileQueue.push({ y, x });

    const std::vector<std::pair<size_t, size_t>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

    while (!tileQueue.empty())
    {
        const auto& [cy, cx] = tileQueue.front();
        tileQueue.pop();

        if (!IsTileInBounds(m_ActiveLayer, cy, cx))
            continue;

        if (m_TileLayers[m_ActiveLayer].Layer[cy][cx].TextureIndex != oldTextureIndex)
            continue;

        m_TileLayers[m_ActiveLayer].Layer[cy][cx].TextureIndex = newTextureIndex;
        m_TileLayers[m_ActiveLayer].Layer[cy][cx].UseTexture = true;

        for (const auto& direction : directions)
        {
            size_t ny = cy + direction.first;
            size_t nx = cx + direction.second;
            tileQueue.push({ ny, nx });
        }
    }
}

void TileLayer::ToggleLayerVisibilty(size_t layer)
{
    m_LayersVisible[layer] = !m_LayersVisible[layer];
}

bool TileLayer::IsLayerVisible(size_t layer)
{
    return m_LayersVisible[layer];
}

const char* TileLayer::GetLayerName(size_t layer)
{
    if (layer < LayerSize())
        return m_TileLayers[layer].Name.c_str();
    return nullptr;
}

void TileLayer::SetLayerName(std::string& name)
{
    m_TileLayers[m_ActiveLayer].Name = name;
}

void TileLayer::SetHoveredTile(size_t y, size_t x)
{
    m_HoveredTile = { m_ActiveLayer, y, x }; 
}

TileData& TileLayer::GetHoveredTile()
{
    return m_TileLayers[m_HoveredTile.L].Layer[m_HoveredTile.Y][m_HoveredTile.X];
}

void TileLayer::SetActiveLayer(size_t layer)
{
    if (layer < LayerSize())
        m_ActiveLayer = layer;
}

void TileLayer::RecordAction(TileAction action)
{
    if (!m_UndoStack.empty())
    {
        const TileAction& top = m_UndoStack.top();
        if (top.L == action.L && top.X == action.X && top.Y == action.Y &&
            top.Curr.TextureIndex == action.Curr.TextureIndex &&
            top.Curr.UseTexture == action.Curr.UseTexture)
        {
            return;
        }
    }

    m_UndoStack.push(action);

    while (!m_RedoStack.empty())
    {
        m_RedoStack.pop();
    }
}

void TileLayer::UndoAction()
{
    if (m_UndoStack.empty())
    {
        return;
    }

    TileAction action = m_UndoStack.top();
    m_UndoStack.pop();

    m_TileLayers[action.L].Layer[action.Y][action.X] = action.Prev;

    m_RedoStack.push(action);

}

void TileLayer::RedoAction()
{
    if (m_RedoStack.empty())
    {
        return;
    }

    TileAction action = m_RedoStack.top();
    m_RedoStack.pop();

    m_TileLayers[action.L].Layer[action.Y][action.X] = action.Curr;

    m_UndoStack.push(action);
}

void TileLayer::Load(const std::string& filename)
{
    m_TileLayers.clear();
    m_TileLayers = TileSerializer::Deserialize(filename);

    m_LayerWidth = m_TileLayers[0].Layer.size();
    m_LayerHeight = m_TileLayers[0].Layer[0].size();

    m_ActiveLayer = LayerSize() - 1;

    m_LayersVisible.clear();
    m_LayersVisible.resize(LayerSize(), true);

    ResetHoveredTile();

    while (!m_UndoStack.empty())
    {
        m_UndoStack.pop();
    }

    while (!m_RedoStack.empty())
    {
        m_RedoStack.pop();
    }
}

void TileLayer::Save(const std::string& filename) const
{
    TileSerializer::Serialize(m_TileLayers, filename);
}

bool TileLayer::IsTileInBounds(size_t layer, size_t y, size_t x) const
{
    if (!(layer < LayerSize()))
    {
        return false;
    }

    if (!(y < LayerHeight()) || !(x < LayerWidth()))
    {
        return false;
    }

    return true;
}