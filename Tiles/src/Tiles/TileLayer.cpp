#include "TileLayer.h"

#include "TileSerializer.h"

#include <stdexcept>
#include <queue>

TileLayer::TileLayer() : m_LayerWidth(0), m_LayerHeight(0) {}

void TileLayer::Init(uint32_t width, uint32_t height)
{
    m_LayerWidth = width;
    m_LayerHeight = height;
    m_Layers.clear(); 
    AddLayer("Layer " + std::to_string(m_Layers.size() + 1));
}

void TileLayer::AddLayer(std::string& name) 
{
    LayerData layer;
    layer.Name = name;

    layer.Layer.resize(m_LayerWidth, std::vector<TileData>(m_LayerHeight));

    for (int y = 0; y < m_LayerHeight; ++y)
    {
        for (int x = 0; x < m_LayerWidth; ++x)
        {
            layer.Layer[y][x] = TileData();
        }
    }

    m_Layers.push_back(layer);
}

void TileLayer::DeleteLayer(uint32_t index) 
{
    if (!m_Layers.empty()) 
    {
        m_Layers.erase(m_Layers.begin() + index);
    }
}

void TileLayer::ClearLayer(uint32_t index)
{
    if (!m_Layers.empty() && index < m_Layers.size())
    {
        auto& layer = m_Layers[index].Layer;
        for (auto& row : layer) 
        {
            for (auto& tile : row) 
            {
                tile = TileData();
            }
        }
    }
}

#include <iostream>

TileData& TileLayer::GetTileData(uint32_t index, uint32_t x, uint32_t y)
{
    if (m_Layers.empty() || index >= m_Layers.size())
        throw std::out_of_range("No active layer.");

    return m_Layers[index].Layer[x][y];
}

void TileLayer::ClearTile(uint32_t index, uint32_t x, uint32_t y)
{
    if (m_Layers.empty() || index >= m_Layers.size())
        throw std::out_of_range("No active layer.");

    m_Layers[index].Layer[x][y] = TileData();
}

#include <iostream>

void TileLayer::FillLayer(uint32_t newTextureIndex, uint32_t index, uint32_t x, uint32_t y)
{
    if (x < 0 || x >= m_LayerWidth || y < 0 || y >= m_LayerHeight)
        return;

    int oldTextureIndex = m_Layers[index].Layer[x][y].TextureIndex;

    if (newTextureIndex == oldTextureIndex)
        return;

    std::queue<std::pair<int, int>> tileQueue;
    tileQueue.push({ x, y });

    const std::vector<std::pair<int, int>> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

    while (!tileQueue.empty())
    {
        auto [x, y] = tileQueue.front();
        tileQueue.pop();

        if (x < 0 || x >= m_LayerWidth || y < 0 || y >= m_LayerHeight)
            continue;

        if (m_Layers[index].Layer[x][y].TextureIndex != oldTextureIndex)
            continue;

        m_Layers[index].Layer[x][y].TextureIndex = newTextureIndex;
        m_Layers[index].Layer[x][y].UseTexture = true;

        for (const auto& direction : directions)
        {
            int newX = x + direction.first;
            int newY = y + direction.second;
            tileQueue.push({ newX, newY });
        }
    }
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
    if (m_UndoStack.empty()) {
        std::cerr << "No actions to undo!" << std::endl;
        return;
    }

    TileAction action = m_UndoStack.top();
    m_UndoStack.pop();

    m_Layers[action.L].Layer[action.Y][action.X] = action.Prev;

    m_RedoStack.push(action);

}

void TileLayer::RedoAction() 
{
    if (m_RedoStack.empty()) 
    {
        std::cerr << "No actions to redo!" << std::endl;
        return;
    }

    TileAction action = m_RedoStack.top();
    m_RedoStack.pop();

    m_Layers[action.L].Layer[action.Y][action.X] = action.Curr;

    m_UndoStack.push(action);
}

void TileLayer::LoadLayers(const std::string& filename)
{
    m_Layers = TileSerializer::Deserialize(filename);
    m_LayerWidth = m_Layers[0].Layer.size();
    m_LayerHeight = m_Layers[0].Layer[0].size();

    while (!m_UndoStack.empty())
    {
        m_UndoStack.pop();
    }

    while (!m_RedoStack.empty())
    {
        m_RedoStack.pop();
    }
}

void TileLayer::SaveLayers(const std::string& filename)
{
    TileSerializer::Serialize(m_Layers, filename); 
}