#pragma once

#include <string>
#include <vector>

#include "Tile.h"

#include "json.hpp"

namespace Tiles
{
	class TileLayer
	{
	public:
		static constexpr int NO_RENDER_GROUP = -1;
		static constexpr int DEFAULT_RENDER_GROUP = 0;

		TileLayer() = default; 
		TileLayer(uint32_t width, uint32_t height);
		~TileLayer() = default;

		void Clear();
		void Resize(uint32_t width, uint32_t height);

		// Tile access
		const Tile& GetTile(size_t x, size_t y) const;
		Tile& GetTile(size_t x, size_t y);

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		void SetWidth(uint32_t width);
		void SetHeight(uint32_t height);

		bool GetVisibility() const { return m_Visible; }
		void SetVisibility(bool visible) { m_Visible = visible; }

		int GetRenderGroup() const { return m_RenderGroupID; }
		void SetRenderGroup(int group) { m_RenderGroupID = group; }
		void DisableRendering() { m_RenderGroupID = NO_RENDER_GROUP; }
		bool IsRenderingEnabled() const { return m_RenderGroupID != NO_RENDER_GROUP; }

		size_t GetTileCount() const { return m_Tiles.size(); }
		bool IsEmpty() const { return m_Tiles.empty(); }
		bool IsValidPosition(size_t x, size_t y) const { return x < m_Width && y < m_Height; }

		auto begin() { return m_Tiles.begin(); }
		auto end() { return m_Tiles.end(); }
		auto begin() const { return m_Tiles.begin(); }
		auto end() const { return m_Tiles.end(); }

		nlohmann::json ToJSON() const;
		static TileLayer FromJSON(const nlohmann::json& jsonLayer);

	private:
		void ResizeInternal(uint32_t width, uint32_t height);

		std::string m_Name = "New Layer";			// Layer name
		uint32_t m_Width = 0;						// Layer width
		uint32_t m_Height = 0;						// Layer height	
		bool m_Visible = true;						// Layer visibility
		int m_RenderGroupID = DEFAULT_RENDER_GROUP;	// Render group ID
		std::vector<Tile> m_Tiles;					// Flat vector of tiles
	};
}