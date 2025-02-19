#pragma once

#include <vector>

#include "Layer.h"
#include "Tile.h"

namespace Tiles
{

	class Layers
	{
	public:
		Layers() = default;
		~Layers() = default;

		void NewLayer();
		void DeleteLayer();
		void ClearLayer();

		void Resize(size_t width, size_t height);
		void Clear();

		void Remove(size_t index);
		void Insert(size_t index, Layer layer);

		// Setters
		void SetActiveLayer(size_t layer) { m_ActiveLayer = layer; }

		// Getters
		size_t GetWidth() const { return m_LayerWidth; }
		size_t GetHeight() const { return m_LayerHeight; }
		size_t GetSize() { return m_Layers.size(); }
		size_t GetActiveLayer() { return m_ActiveLayer; }

		Layer& GetLayer(size_t index) { return m_Layers[index]; }
		Tile& GetTile(size_t index, size_t y, size_t x) { return m_Layers[index].GetTile(y, x); }
	private:
		bool IsLayerInBounds(size_t layer);
	private:
		size_t m_LayerWidth = 0;		// Height of each layer
		size_t m_LayerHeight = 0;		// Width of each layer
		size_t m_ActiveLayer = 0;		// Layer that can be edited.

		std::vector<Layer> m_Layers;	// Layers
	};

}