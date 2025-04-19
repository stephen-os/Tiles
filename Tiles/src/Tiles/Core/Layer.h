#pragma once

#include <string>
#include <vector>
#include "Tile.h"

namespace Tiles
{
	class Layer
	{
	public:
		Layer(size_t width = 0, size_t height = 0, const std::string& name = "New Layer");
		~Layer() = default;

		void Clear();

		// Setters
		void SetName(const std::string& name) { m_Name = name; }
		void ToggleVisibility() { m_Visible = !m_Visible; }

		// Getters
		const std::string& GetName() const { return m_Name; }
		size_t GetWidth() const { return m_Width; }
		size_t GetHeight() const { return m_Height; }
		bool GetVisibility() const { return m_Visible; }
		const Tile& GetTile(size_t y, size_t x) const { return m_Tiles[y * m_Width + x]; }
		Tile& GetTile(size_t y, size_t x) { return m_Tiles[y * m_Width + x]; }

		// Render Group
		const int GetRenderGroup() const { return m_RenderGroup; }
		void SetRenderGroup(int group) { m_RenderGroup = group; }
	private:
		std::string m_Name;			// Layer name
		size_t m_Width;				// Layer width
		size_t m_Height; 			// Layer height	
		bool m_Visible = true;		// Layer visibility
		int m_RenderGroup = 0;		// By default, all groups are set to 0. -1 is reserved for no group. 
		std::vector<Tile> m_Tiles;	// Flat vector of tiles
	};
}