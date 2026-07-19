#include "Domain/TileLayer.h"

#include "Domain/Constants.h"
#include "Core/Logger.h"

#include <climits>

namespace Tiles
{
	// Returned by GetTile for unpainted coordinates.
	static const Tile s_EmptyTile;

	// Removes every painted tile.
	void TileLayer::Clear()
	{
		m_Tiles.clear();
	}

	// The tile at (x, y), or the shared empty tile if the cell is unpainted.
	const Tile& TileLayer::GetTile(int x, int y) const
	{
		auto it = m_Tiles.find({ x, y });
		return it != m_Tiles.end() ? it->second : s_EmptyTile;
	}

	// Whether (x, y) holds a painted tile.
	bool TileLayer::HasTile(int x, int y) const
	{
		return m_Tiles.find({ x, y }) != m_Tiles.end();
	}

	// Paints (x, y); an unpainted tile clears the cell so only painted tiles are stored.
	void TileLayer::SetTile(int x, int y, const Tile& tile)
	{
		if (tile.IsPainted())
			m_Tiles[{ x, y }] = tile;
		else
			m_Tiles.erase({ x, y });
	}

	// Clears a single cell.
	void TileLayer::EraseTile(int x, int y)
	{
		m_Tiles.erase({ x, y });
	}

	// Sets the layer name, falling back to a default for an empty name.
	void TileLayer::SetName(const std::string& name)
	{
		m_Name = name.empty() ? "New Layer" : name;
	}

	// Inclusive bounding box of painted tiles, or nullopt when the layer is empty.
	std::optional<glm::ivec4> TileLayer::GetBounds() const
	{
		if (m_Tiles.empty())
			return std::nullopt;

		glm::ivec2 min(INT_MAX);
		glm::ivec2 max(INT_MIN);
		for (const auto& [coord, tile] : m_Tiles)
		{
			min = glm::min(min, coord);
			max = glm::max(max, coord);
		}
		return glm::ivec4(min.x, min.y, max.x, max.y);
	}

	// Serializes the layer's metadata and its painted cells (sparse).
	nlohmann::json TileLayer::ToJSON() const
	{
		nlohmann::json jsonLayer;

		jsonLayer[JSON::TileLayer::Name] = GetName();
		jsonLayer[JSON::TileLayer::Visible] = GetVisibility();
		jsonLayer[JSON::TileLayer::RenderGroup] = GetRenderGroup();

		// Store only painted cells, each tagged with its coordinate.
		nlohmann::json tilesArray = nlohmann::json::array();
		for (const auto& [coord, tile] : m_Tiles)
		{
			nlohmann::json entry = tile.ToJSON();
			entry[JSON::TileLayer::TileX] = coord.x;
			entry[JSON::TileLayer::TileY] = coord.y;
			tilesArray.push_back(std::move(entry));
		}
		jsonLayer[JSON::TileLayer::Tiles] = tilesArray;

		return jsonLayer;
	}

	// Reconstructs a layer from JSON, supporting both the sparse and legacy dense
	// tile formats and keeping defaults for any missing metadata.
	TileLayer TileLayer::FromJSON(const nlohmann::json& jsonLayer)
	{
		TileLayer layer;

		if (jsonLayer.contains(JSON::TileLayer::Name))
			layer.SetName(jsonLayer[JSON::TileLayer::Name].get<std::string>());

		if (jsonLayer.contains(JSON::TileLayer::Visible))
			layer.SetVisibility(jsonLayer[JSON::TileLayer::Visible].get<bool>());

		if (jsonLayer.contains(JSON::TileLayer::RenderGroup))
			layer.SetRenderGroup(static_cast<RenderGroup>(jsonLayer[JSON::TileLayer::RenderGroup].get<int>()));

		if (!jsonLayer.contains(JSON::TileLayer::Tiles))
			return layer;

		const auto& tilesArray = jsonLayer[JSON::TileLayer::Tiles];

		// Legacy dense format: a width/height plus "tiles" as a 2D array of rows.
		// Load only painted cells into the sparse map.
		const bool legacyDense = jsonLayer.contains(JSON::TileLayer::Width)
			&& !tilesArray.empty() && tilesArray.front().is_array();

		if (legacyDense)
		{
			for (int y = 0; y < static_cast<int>(tilesArray.size()); ++y)
			{
				const auto& rowArray = tilesArray[y];
				for (int x = 0; x < static_cast<int>(rowArray.size()); ++x)
				{
					Tile tile = Tile::FromJSON(rowArray[x]);
					if (tile.IsPainted())
						layer.SetTile(x, y, tile);
				}
			}
		}
		else
		{
			// Skip only the malformed tile, not the whole layer: one bad entry must
			// not silently discard thousands of good tiles (which the next save would
			// then make permanent).
			size_t skipped = 0;
			for (const auto& entry : tilesArray)
			{
				try
				{
					int x = entry.at(JSON::TileLayer::TileX).get<int>();
					int y = entry.at(JSON::TileLayer::TileY).get<int>();
					layer.SetTile(x, y, Tile::FromJSON(entry));
				}
				catch (const std::exception&)
				{
					++skipped;
				}
			}

			if (skipped > 0)
				TILES_ENGINE_WARN("TileLayer::FromJSON: Skipped {} malformed tile entries while loading a layer.", skipped);
		}

		return layer;
	}
}
