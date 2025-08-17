#include "LayerStack.h"

#include "Lumina/Core/Assert.h"

namespace Tiles
{
    namespace LayerStackFields
    {
        static constexpr const char* WIDTH = "width";
        static constexpr const char* HEIGHT = "height";
        static constexpr const char* LAYERS = "layers";
    }

    LayerStack::LayerStack(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

    void LayerStack::AddLayer(const std::string& name)
    {
        m_Layers.emplace_back(m_Width, m_Height);
        m_Layers.back().SetName(name);
    }

    void LayerStack::RemoveLayer(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index for removal");
        m_Layers.erase(m_Layers.begin() + index);
    }

    void LayerStack::ClearLayer(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index for clearing");
        m_Layers[index].Clear();
    }

    void LayerStack::InsertLayer(size_t index, const std::string& name)
    {
        LUMINA_ASSERT(index <= m_Layers.size(), "Invalid insertion index");
        auto it = m_Layers.insert(m_Layers.begin() + index, TileLayer(m_Width, m_Height));
        it->SetName(name);
    }

    void LayerStack::ReplaceLayer(size_t index, const TileLayer& layer)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index for replacement");
        m_Layers[index] = layer;
        m_Layers[index].Resize(m_Width, m_Height);
    }

    void LayerStack::SwapLayers(size_t indexA, size_t indexB)
    {
        LUMINA_ASSERT(IsValidLayerIndex(indexA), "Invalid layer index A");
        LUMINA_ASSERT(IsValidLayerIndex(indexB), "Invalid layer index B");

        if (indexA != indexB)
        {
            std::swap(m_Layers[indexA], m_Layers[indexB]);
        }
    }

    void LayerStack::ClearAllLayers()
    {
        m_Layers.clear();
    }

    void LayerStack::MoveLayerUp(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index for move up");
        if (index > 0)
        {
            std::swap(m_Layers[index], m_Layers[index - 1]);
        }
    }

    void LayerStack::MoveLayerDown(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index for move down");
        if (index < m_Layers.size() - 1)
        {
            std::swap(m_Layers[index], m_Layers[index + 1]);
        }
    }

    void LayerStack::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;

        for (auto& layer : m_Layers)
        {
            layer.Resize(width, height);
        }
    }

    TileLayer& LayerStack::GetLayer(size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index");
        return m_Layers[index];
    }

    const TileLayer& LayerStack::GetLayer(size_t index) const
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index");
        return m_Layers[index];
    }

    Tile& LayerStack::GetTile(size_t x, size_t y, size_t index)
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index");
        return m_Layers[index].GetTile(x, y);  
    }

    const Tile& LayerStack::GetTile(size_t x, size_t y, size_t index) const
    {
        LUMINA_ASSERT(IsValidLayerIndex(index), "Invalid layer index");
        return m_Layers[index].GetTile(x, y); 
    }

    nlohmann::json LayerStack::ToJSON() const
    {
        nlohmann::json jsonLayerStack;

        jsonLayerStack[LayerStackFields::WIDTH] = GetWidth();
        jsonLayerStack[LayerStackFields::HEIGHT] = GetHeight();

        nlohmann::json layersArray = nlohmann::json::array();
        for (const auto& layer : m_Layers)
        {
            layersArray.push_back(layer.ToJSON());
        }
        jsonLayerStack[LayerStackFields::LAYERS] = layersArray;

        return jsonLayerStack;
    }

    LayerStack LayerStack::FromJSON(const nlohmann::json& jsonLayerStack)
    {
        uint32_t width = jsonLayerStack.at(LayerStackFields::WIDTH).get<uint32_t>();
        uint32_t height = jsonLayerStack.at(LayerStackFields::HEIGHT).get<uint32_t>();

        LayerStack layerStack(width, height);

        if (jsonLayerStack.contains(LayerStackFields::LAYERS))
        {
            const auto& layersArray = jsonLayerStack[LayerStackFields::LAYERS];
            for (const auto& layerJson : layersArray)
            {
                TileLayer layer = TileLayer::FromJSON(layerJson);

                if (layer.GetWidth() != width || layer.GetHeight() != height)
                {
                    layer.Resize(width, height);
                }

                layerStack.m_Layers.push_back(layer);
            }
        }

        return layerStack;
    }
}