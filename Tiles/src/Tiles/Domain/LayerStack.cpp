#include "Domain/LayerStack.h"

#include "Core/Assert.h"
#include "Core/Logger.h"
#include "Domain/Constants.h"

#include <algorithm>

namespace Tiles
{
	// Appends a new empty layer on top.
	void LayerStack::AddLayer(const std::string& name)
	{
		m_Layers.emplace_back();
		std::string layerName = name.empty() ? "New Layer" : name;
		m_Layers.back().SetName(layerName);

		TILES_ENGINE_INFO("LayerStack::AddLayer: Added layer '{}' (total layers: {})", layerName, m_Layers.size());
	}

	// Removes the layer at index; a no-op for an invalid index.
	void LayerStack::RemoveLayer(size_t index)
	{
		if (!IsValidLayerIndex(index))
		{
			TILES_ENGINE_INFO("LayerStack::RemoveLayer: Invalid index {} (layer count: {})", index, m_Layers.size());
			return;
		}

		std::string layerName = m_Layers[index].GetName();
		m_Layers.erase(m_Layers.begin() + index);
		TILES_ENGINE_INFO("LayerStack::RemoveLayer: Removed layer '{}' (remaining: {})", layerName, m_Layers.size());
	}

	// Clears the painted tiles of the layer at index; a no-op for an invalid index.
	void LayerStack::ClearLayer(size_t index)
	{
		if (!IsValidLayerIndex(index))
		{
			TILES_ENGINE_INFO("LayerStack::ClearLayer: Attempted to clear layer at invalid index {} (layer count: {})", index, m_Layers.size());
			return;
		}

		m_Layers[index].Clear();
	}

	// Inserts a new empty layer at index, clamping past-the-end to append.
	void LayerStack::InsertLayer(size_t index, const std::string& name)
	{
		if (index > m_Layers.size())
			index = m_Layers.size();

		std::string layerName = name.empty() ? "New Layer" : name;
		auto it = m_Layers.insert(m_Layers.begin() + index, TileLayer());
		it->SetName(layerName);

		TILES_ENGINE_INFO("LayerStack::InsertLayer: Inserted layer '{}' at index {} (total: {})", layerName, index, m_Layers.size());
	}

	// Overwrites the layer at index; a no-op for an invalid index.
	void LayerStack::ReplaceLayer(size_t index, const TileLayer& layer)
	{
		if (!IsValidLayerIndex(index))
		{
			TILES_ENGINE_INFO("LayerStack::ReplaceLayer: Attempted to replace layer at invalid index {} (layer count: {})", index, m_Layers.size());
			return;
		}

		m_Layers[index] = layer;
	}

	// Removes every layer.
	void LayerStack::ClearAllLayers()
	{
		size_t layerCount = m_Layers.size();
		m_Layers.clear();

		if (layerCount > 0)
			TILES_ENGINE_INFO("LayerStack::ClearAllLayers: Cleared {} layers", layerCount);
	}

	// Swaps the layer at index with the one above it.
	void LayerStack::MoveLayerUp(size_t index)
	{
		if (!IsValidLayerIndex(index))
		{
			TILES_ENGINE_INFO("LayerStack::MoveLayerUp: Invalid layer index {} (layer count: {})", index, m_Layers.size());
			return;
		}

		if (index == 0)
			return;

		std::swap(m_Layers[index], m_Layers[index - 1]);
	}

	// Swaps the layer at index with the one below it.
	void LayerStack::MoveLayerDown(size_t index)
	{
		if (!IsValidLayerIndex(index))
		{
			TILES_ENGINE_INFO("LayerStack::MoveLayerDown: Invalid layer index {} (layer count: {})", index, m_Layers.size());
			return;
		}

		if (index >= m_Layers.size() - 1)
			return;

		std::swap(m_Layers[index], m_Layers[index + 1]);
	}

	// The layer at index; asserts the index is valid.
	TileLayer& LayerStack::GetLayer(size_t index)
	{
		TILES_ASSERT(IsValidLayerIndex(index), "LayerStack::GetLayer: Invalid layer index {} (layer count: {})", index, m_Layers.size());
		return m_Layers[index];
	}

	// The layer at index; asserts the index is valid.
	const TileLayer& LayerStack::GetLayer(size_t index) const
	{
		TILES_ASSERT(IsValidLayerIndex(index), "LayerStack::GetLayer: Invalid layer index {} (layer count: {})", index, m_Layers.size());
		return m_Layers[index];
	}

	// The tile at (x, y) on layer index; asserts the index is valid.
	const Tile& LayerStack::GetTile(int x, int y, size_t index) const
	{
		TILES_ASSERT(IsValidLayerIndex(index), "LayerStack::GetTile: Invalid layer index {} (layer count: {})", index, m_Layers.size());
		return m_Layers[index].GetTile(x, y);
	}

	// Paints (x, y) on layer index; asserts the index is valid.
	void LayerStack::SetTile(int x, int y, size_t index, const Tile& tile)
	{
		TILES_ASSERT(IsValidLayerIndex(index), "LayerStack::SetTile: Invalid layer index {} (layer count: {})", index, m_Layers.size());
		m_Layers[index].SetTile(x, y, tile);
	}

	// Inclusive bounding box of painted tiles across all layers, or nullopt when
	// nothing is painted.
	std::optional<glm::ivec4> LayerStack::GetBounds() const
	{
		std::optional<glm::ivec4> bounds;
		for (const auto& layer : m_Layers)
		{
			auto layerBounds = layer.GetBounds();
			if (!layerBounds)
				continue;

			if (!bounds)
			{
				bounds = layerBounds;
			}
			else
			{
				bounds->x = std::min(bounds->x, layerBounds->x);
				bounds->y = std::min(bounds->y, layerBounds->y);
				bounds->z = std::max(bounds->z, layerBounds->z);
				bounds->w = std::max(bounds->w, layerBounds->w);
			}
		}
		return bounds;
	}

	// Serializes every layer in order.
	nlohmann::json LayerStack::ToJSON() const
	{
		nlohmann::json jsonLayerStack;

		nlohmann::json layersArray = nlohmann::json::array();
		for (const auto& layer : m_Layers)
			layersArray.push_back(layer.ToJSON());
		jsonLayerStack[JSON::LayerStack::TileLayers] = layersArray;

		return jsonLayerStack;
	}

	// Rebuilds a stack from JSON, skipping (and logging) any layer that fails to parse.
	LayerStack LayerStack::FromJSON(const nlohmann::json& jsonLayerStack)
	{
		LayerStack layerStack;

		if (jsonLayerStack.contains(JSON::LayerStack::TileLayers))
		{
			const auto& layersArray = jsonLayerStack[JSON::LayerStack::TileLayers];
			size_t loadedCount = 0;

			for (const auto& layerJson : layersArray)
			{
				try
				{
					layerStack.m_Layers.push_back(TileLayer::FromJSON(layerJson));
					loadedCount++;
				}
				catch (const std::exception& e)
				{
					TILES_ENGINE_INFO("LayerStack::FromJSON: Failed to load layer: {}", e.what());
				}
			}

			if (loadedCount != layersArray.size())
				TILES_ENGINE_INFO("LayerStack::FromJSON: Loaded {} out of {} layers", loadedCount, layersArray.size());
		}

		return layerStack;
	}
}
