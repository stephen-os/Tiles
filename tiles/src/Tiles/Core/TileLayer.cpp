#include "TileLayer.h"

#include "Lumina/Core/Assert.h"

#include "Constants.h"

namespace Tiles
{
	TileLayer::TileLayer(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
	{
		LUMINA_ASSERT((width * height) >= 0, "TileLayer::TileLayer: Size must be greater than 0");
		m_Tiles.resize(width * height);
	}

	void TileLayer::Clear()
	{
		for (auto& tile : m_Tiles)
		{
			tile.Reset();
		}
	}

	void TileLayer::Resize(uint32_t width, uint32_t height)
	{
		if (width == m_Width && height == m_Height)
			return;

		ResizeInternal(width, height);
	}

	const Tile& TileLayer::GetTile(size_t x, size_t y) const
	{
		LUMINA_ASSERT(IsValidPosition(x, y), "Const TileLayer::GetTile: Tile position out of bounds");
		return m_Tiles[y * m_Width + x];
	}

	Tile& TileLayer::GetTile(size_t x, size_t y)
	{
		LUMINA_ASSERT(IsValidPosition(x, y), "TileLayer::GetTile: Tile position out of bounds");
		return m_Tiles[y * m_Width + x];
	}

	void TileLayer::SetWidth(uint32_t width)
	{
		if (width != m_Width)
		{
			ResizeInternal(width, m_Height);
		}
	}

	void TileLayer::SetHeight(uint32_t height)
	{
		if (height != m_Height)
		{
			ResizeInternal(m_Width, height);
		}
	}

	void TileLayer::ResizeInternal(uint32_t width, uint32_t height)
	{
		std::vector<Tile> newTiles(width * height);

		uint32_t copyWidth = std::min(width, m_Width);
		uint32_t copyHeight = std::min(height, m_Height);

		for (uint32_t y = 0; y < copyHeight; y++)
		{
			for (uint32_t x = 0; x < copyWidth; x++)
			{
				newTiles[y * width + x] = m_Tiles[y * m_Width + x];
			}
		}

		m_Tiles = std::move(newTiles);
		m_Width = width;
		m_Height = height;
	}

	nlohmann::json TileLayer::ToJSON() const
	{
		nlohmann::json jsonLayer;

		jsonLayer[JSON::TileLayer::Name] = GetName();
		jsonLayer[JSON::TileLayer::Width] = GetWidth();
		jsonLayer[JSON::TileLayer::Height] = GetHeight();
		jsonLayer[JSON::TileLayer::Visible] = GetVisibility();
		jsonLayer[JSON::TileLayer::RenderGroup] = GetRenderGroup();

		nlohmann::json tilesArray = nlohmann::json::array();
		for (uint32_t y = 0; y < GetHeight(); y++)
		{
			nlohmann::json rowArray = nlohmann::json::array();
			for (uint32_t x = 0; x < GetWidth(); x++)
			{
				const Tile& tile = GetTile(x, y);
				rowArray.push_back(tile.ToJSON());
			}
			tilesArray.push_back(rowArray);
		}
		jsonLayer[JSON::TileLayer::Tiles] = tilesArray;

		return jsonLayer;
	}

	TileLayer TileLayer::FromJSON(const nlohmann::json& jsonLayer)
	{
		uint32_t width = jsonLayer.at(JSON::TileLayer::Width).get<uint32_t>();
		uint32_t height = jsonLayer.at(JSON::TileLayer::Height).get<uint32_t>();

		TileLayer layer(width, height);

		if (jsonLayer.contains(JSON::TileLayer::Name))
		{
			std::string name = jsonLayer[JSON::TileLayer::Name].get<std::string>();
			layer.SetName(name);
		}

		if (jsonLayer.contains(JSON::TileLayer::Visible))
		{
			bool visible = jsonLayer[JSON::TileLayer::Visible].get<bool>();
			layer.SetVisibility(visible);
		}

		if (jsonLayer.contains(JSON::TileLayer::RenderGroup))
		{
			int renderGroup = jsonLayer[JSON::TileLayer::RenderGroup].get<int>();
			layer.SetRenderGroup(renderGroup);
		}

		if (jsonLayer.contains(JSON::TileLayer::Tiles))
		{
			const auto& tilesArray = jsonLayer[JSON::TileLayer::Tiles];
			for (uint32_t y = 0; y < height && y < tilesArray.size(); y++)
			{
				const auto& rowArray = tilesArray[y];
				for (uint32_t x = 0; x < width && x < rowArray.size(); x++)
				{
					Tile tile = Tile::FromJSON(rowArray[x]);
					layer.GetTile(x, y) = tile;
				}
			}
		}

		return layer;
	}
}