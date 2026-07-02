#pragma once

#include <vector>
#include <string>

#include "TileLayer.h"

#include "json.hpp"

namespace Tiles
{
    /// An ordered collection of equally-sized tile layers sharing one grid
    /// dimension. All layers are kept at the stack's width/height.
    class LayerStack
    {
    public:
        nlohmann::json ToJSON() const;
        /// Rebuilds a stack from JSON, resizing each loaded layer to the stack's
        /// dimensions and skipping any layer that fails to parse.
        static LayerStack FromJSON(const nlohmann::json& jsonLayerStack);

        /// @param width,height Grid size in tiles; clamped to Grid::MaxDimension.
        LayerStack(uint32_t width = 0, uint32_t height = 0);
        ~LayerStack() = default;

        void AddLayer(const std::string& name = "New Layer");
        void RemoveLayer(size_t index);
        void ClearLayer(size_t index);
        void InsertLayer(size_t index, const std::string& name = "New Layer");
        /// Overwrites the layer at index, resizing it to the stack dimensions if
        /// the incoming layer differs. No-op for an invalid index.
        void ReplaceLayer(size_t index, const TileLayer& layer);
        void ClearAllLayers();

        void MoveLayerUp(size_t index);
        void MoveLayerDown(size_t index);

        /// Resizes the stack and every layer to the new dimensions (clamped).
        void Resize(uint32_t width, uint32_t height);
        bool IsEmpty() const { return m_Layers.empty(); }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        size_t GetLayerCount() const { return m_Layers.size(); }

        TileLayer& GetLayer(size_t index);
        const TileLayer& GetLayer(size_t index) const;

        Tile& GetTile(size_t x, size_t y, size_t index);
        const Tile& GetTile(size_t x, size_t y, size_t index) const;

        auto begin() { return m_Layers.begin(); }
        auto end() { return m_Layers.end(); }
        auto begin() const { return m_Layers.begin(); }
        auto end() const { return m_Layers.end(); }

        bool IsValidLayerIndex(size_t index) const { return index < m_Layers.size(); }
    
    private:
        uint32_t m_Width = 16;                          // Width of the layer stack in tiles
        uint32_t m_Height = 16;                         // Height of the layer stack in tiles
        std::vector<TileLayer> m_Layers;                // Collection of tile layers ordered from bottom to top
    };
}