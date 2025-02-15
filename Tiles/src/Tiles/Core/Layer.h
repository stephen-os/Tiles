#pragma once

#include <string>
#include <vector>

#include "Tile.h"

namespace Tiles
{

	class Layer
	{
	public:
		Layer() = default;
		Layer(size_t width, size_t height, const std::string& name);
		~Layer() = default;

		// Utility
		void Clear();

		// Setters
		void SetName(const std::string& name) { m_Name = name; }
		void ToggleVisibility() { m_Visible = !m_Visible; }

		// Getters
		const std::string& GetName() const { return m_Name; }
		size_t GetWidth() const { return m_Width; }
		size_t GetHeight() const { return m_Height; }
		bool GetVisibility() const { return m_Visible; }

		Tile& GetTile(size_t y, size_t x) { return m_Tiles[y * m_Width + x]; }
		const Tile& GetTile(size_t y, size_t x) const { return m_Tiles[y * m_Width + x]; }
	private:
		std::string m_Name = "New Layer";	// Layer name
		size_t m_Width = 0;					// Layer width
		size_t m_Height = 0; 				// Layer height	

		bool m_Visible = true;				// Layer visibility

		std::vector<Tile> m_Tiles;			// Flat vector
	};

}