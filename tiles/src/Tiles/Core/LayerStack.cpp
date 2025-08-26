#include "LayerStack.h"
#include "Lumina/Core/Assert.h"
#include "Lumina/Core/Log.h"
#include "Constants.h"

namespace Tiles
{
    LayerStack::LayerStack(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
    {
        if (width == 0 || height == 0)
        {
            LUMINA_LOG_INFO("LayerStack::LayerStack: Warning - LayerStack created with zero dimensions ({}x{})", width, height);
        }
    }

    void LayerStack::AddLayer(const std::string& name)
    {
        m_Layers.emplace_back(m_Width, m_Height);
        std::string layerName = name.empty() ? "New Layer" : name;
        m_Layers.back().SetName(layerName);

        LUMINA_LOG_INFO("LayerStack::AddLayer: Added layer '{}' (total layers: {})", layerName, m_Layers.size());
    }

    void LayerStack::RemoveLayer(size_t index)
    {
        if (!IsValidLayerIndex(index))
        {
            LUMINA_LOG_INFO("LayerStack::RemoveLayer: Invalid index {} (layer count: {})", index, m_Layers.size());
            return;
        }

        std::string layerName = m_Layers[index].GetName();
        m_Layers.erase(m_Layers.begin() + index);
        LUMINA_LOG_INFO("LayerStack::RemoveLayer: Removed layer '{}' (remaining: {})", layerName, m_Layers.size());
    }

    void LayerStack::ClearLayer(size_t index)
    {
        if (!IsValidLayerIndex(index))
        {
            LUMINA_LOG_INFO("LayerStack::ClearLayer: Attempted to clear layer at invalid index {} (layer count: {})", index, m_Layers.size());
            return;
        }

        LUMINA_LOG_INFO("LayerStack::ClearLayer: Clearing layer at index {}", index);
        m_Layers[index].Clear();
    }

    void LayerStack::InsertLayer(size_t index, const std::string& name)
    {
        if (index > m_Layers.size())
        {
            index = m_Layers.size();
        }

        std::string layerName = name.empty() ? "New Layer" : name;
        auto it = m_Layers.insert(m_Layers.begin() + index, TileLayer(m_Width, m_Height));
        it->SetName(layerName);

        LUMINA_LOG_INFO("LayerStack::InsertLayer: Inserted layer '{}' at index {} (total: {})", layerName, index, m_Layers.size());
    }

    void LayerStack::ReplaceLayer(size_t index, const TileLayer& layer)
    {
        if (!IsValidLayerIndex(index))
        {
            LUMINA_LOG_INFO("LayerStack::ReplaceLayer: Attempted to replace layer at invalid index {} (layer count: {})", index, m_Layers.size());
            return;
        }

        LUMINA_LOG_INFO("LayerStack::ReplaceLayer: Replacing layer at index {}", index);
        m_Layers[index] = layer;

        if (layer.GetWidth() != m_Width || layer.GetHeight() != m_Height)
        {
            LUMINA_LOG_INFO("LayerStack::ReplaceLayer: Resizing replaced layer from {}x{} to {}x{}", layer.GetWidth(), layer.GetHeight(), m_Width, m_Height);
            m_Layers[index].Resize(m_Width, m_Height);
        }
    }

    void LayerStack::SwapLayers(size_t indexA, size_t indexB)
    {
        if (!IsValidLayerIndex(indexA))
        {
            LUMINA_LOG_INFO("LayerStack::SwapLayers: Invalid layer index A: {} (layer count: {})", indexA, m_Layers.size());
            return;
        }

        if (!IsValidLayerIndex(indexB))
        {
            LUMINA_LOG_INFO("LayerStack::SwapLayers: Invalid layer index B: {} (layer count: {})", indexB, m_Layers.size());
            return;
        }

        if (indexA == indexB)
        {
            LUMINA_LOG_INFO("LayerStack::SwapLayers: Attempted to swap layer {} with itself - no action taken", indexA);
            return;
        }

        LUMINA_LOG_INFO("LayerStack::SwapLayers: Swapping layers at indices {} and {}", indexA, indexB);
        std::swap(m_Layers[indexA], m_Layers[indexB]);
    }

    void LayerStack::ClearAllLayers()
    {
        size_t layerCount = m_Layers.size();
        m_Layers.clear();

        if (layerCount > 0)
        {
            LUMINA_LOG_INFO("LayerStack::ClearAllLayers: Cleared {} layers", layerCount);
        }
    }

    void LayerStack::MoveLayerUp(size_t index)
    {
        if (!IsValidLayerIndex(index))
        {
            LUMINA_LOG_INFO("LayerStack::MoveLayerUp: Invalid layer index {} (layer count: {})", index, m_Layers.size());
            return;
        }

        if (index == 0)
        {
            LUMINA_LOG_INFO("LayerStack::MoveLayerUp: Layer at index {} is already at the top", index);
            return;
        }

        LUMINA_LOG_INFO("LayerStack::MoveLayerUp: Moving layer from index {} to {}", index, index - 1);
        std::swap(m_Layers[index], m_Layers[index - 1]);
    }

    void LayerStack::MoveLayerDown(size_t index)
    {
        if (!IsValidLayerIndex(index))
        {
            LUMINA_LOG_INFO("LayerStack::MoveLayerDown: Invalid layer index {} (layer count: {})", index, m_Layers.size());
            return;
        }

        if (index >= m_Layers.size() - 1)
        {
            LUMINA_LOG_INFO("LayerStack::MoveLayerDown: Layer at index {} is already at the bottom", index);
            return;
        }

        LUMINA_LOG_INFO("LayerStack::MoveLayerDown: Moving layer from index {} to {}", index, index + 1);
        std::swap(m_Layers[index], m_Layers[index + 1]);
    }

    void LayerStack::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            LUMINA_LOG_INFO("LayerStack::Resize: Warning - Resizing LayerStack to zero dimensions ({}x{})", width, height);
        }

        LUMINA_LOG_INFO("LayerStack::Resize: Resizing LayerStack from {}x{} to {}x{} ({} layers)", m_Width, m_Height, width, height, m_Layers.size());

        m_Width = width;
        m_Height = height;

        for (size_t i = 0; i < m_Layers.size(); ++i)
        {
            m_Layers[i].Resize(width, height);
        }

        LUMINA_LOG_INFO("LayerStack::Resize: LayerStack resize completed");
    }

    TileLayer& LayerStack::GetLayer(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "LayerStack::GetLayer: Invalid layer index {} (layer count: {})", index, m_Layers.size());
        return m_Layers[index];
    }

    const TileLayer& LayerStack::GetLayer(size_t index) const
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "LayerStack::GetLayer: Invalid layer index {} (layer count: {})", index, m_Layers.size());
        return m_Layers[index];
    }

    Tile& LayerStack::GetTile(size_t x, size_t y, size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "LayerStack::GetTile: Invalid layer index {} (layer count: {})", index, m_Layers.size());
        return m_Layers[index].GetTile(x, y);
    }

    const Tile& LayerStack::GetTile(size_t x, size_t y, size_t index) const
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "LayerStack::GetTile: Invalid layer index {} (layer count: {})", index, m_Layers.size());
        return m_Layers[index].GetTile(x, y);
    }

    nlohmann::json LayerStack::ToJSON() const
    {
        nlohmann::json jsonLayerStack;

        jsonLayerStack[JSON::LayerStack::Width] = GetWidth();
        jsonLayerStack[JSON::LayerStack::Height] = GetHeight();

        nlohmann::json layersArray = nlohmann::json::array();
        for (const auto& layer : m_Layers)
        {
            layersArray.push_back(layer.ToJSON());
        }
        jsonLayerStack[JSON::LayerStack::TileLayers] = layersArray;

        return jsonLayerStack;
    }

    LayerStack LayerStack::FromJSON(const nlohmann::json& jsonLayerStack)
    {
        if (!jsonLayerStack.contains(JSON::LayerStack::Width) || !jsonLayerStack.contains(JSON::LayerStack::Height))
        {
            LUMINA_LOG_INFO("LayerStack::FromJSON: JSON missing required Width or Height fields");
            return LayerStack(0, 0);
        }

        uint32_t width = jsonLayerStack.at(JSON::LayerStack::Width).get<uint32_t>();
        uint32_t height = jsonLayerStack.at(JSON::LayerStack::Height).get<uint32_t>();

        LayerStack layerStack(width, height);

        if (jsonLayerStack.contains(JSON::LayerStack::TileLayers))
        {
            const auto& layersArray = jsonLayerStack[JSON::LayerStack::TileLayers];
            size_t loadedCount = 0;

            for (const auto& layerJson : layersArray)
            {
                try
                {
                    TileLayer layer = TileLayer::FromJSON(layerJson);

                    if (layer.GetWidth() != width || layer.GetHeight() != height)
                    {
                        layer.Resize(width, height);
                    }

                    layerStack.m_Layers.push_back(layer);
                    loadedCount++;
                }
                catch (const std::exception& e)
                {
                    LUMINA_LOG_INFO("LayerStack::FromJSON: Failed to load layer: {}", e.what());
                }
            }

            if (loadedCount != layersArray.size())
            {
                LUMINA_LOG_INFO("LayerStack::FromJSON: Loaded {} out of {} layers", loadedCount, layersArray.size());
            }
        }

        return layerStack;
    }
}