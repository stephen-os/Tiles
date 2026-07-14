#pragma once

#include "Domain/TileLayer.h"

#include "json.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <string>
#include <vector>

namespace Tiles
{
	// An ordered collection of sparse tile layers (bottom to top). The board is
	// unbounded; layers share no fixed dimension.
	class LayerStack
	{
	public:
		[[nodiscard]] nlohmann::json ToJSON() const;
		// Rebuilds a stack from JSON, skipping any layer that fails to parse.
		[[nodiscard]] static LayerStack FromJSON(const nlohmann::json& jsonLayerStack);

		LayerStack() = default;
		~LayerStack() = default;

		void AddLayer(const std::string& name = "New Layer");
		void RemoveLayer(size_t index);
		void ClearLayer(size_t index);
		void InsertLayer(size_t index, const std::string& name = "New Layer");
		// Overwrites the layer at index. No-op for an invalid index.
		void ReplaceLayer(size_t index, const TileLayer& layer);
		void ClearAllLayers();

		void MoveLayerUp(size_t index);
		void MoveLayerDown(size_t index);

		[[nodiscard]] bool IsEmpty() const { return m_Layers.empty(); }
		[[nodiscard]] size_t GetLayerCount() const { return m_Layers.size(); }

		[[nodiscard]] TileLayer& GetLayer(size_t index);
		[[nodiscard]] const TileLayer& GetLayer(size_t index) const;

		// The tile at (x, y) on layer @p index, or a shared empty tile if absent.
		[[nodiscard]] const Tile& GetTile(int x, int y, size_t index) const;
		// Paints (x, y) on layer @p index; an unpainted tile clears the cell.
		void SetTile(int x, int y, size_t index, const Tile& tile);

		// Inclusive bounding box of painted tiles across all layers as
		// (minX, minY, maxX, maxY), or nullopt when nothing is painted.
		[[nodiscard]] std::optional<glm::ivec4> GetBounds() const;

		[[nodiscard]] auto begin() { return m_Layers.begin(); }
		[[nodiscard]] auto end() { return m_Layers.end(); }
		[[nodiscard]] auto begin() const { return m_Layers.begin(); }
		[[nodiscard]] auto end() const { return m_Layers.end(); }

		[[nodiscard]] bool IsValidLayerIndex(size_t index) const { return index < m_Layers.size(); }

	private:
		std::vector<TileLayer> m_Layers;                // layers ordered bottom to top
	};
}
