#pragma once

#include <vector>
#include <limits>

#include "Layer.h"
#include "Tile.h"

namespace Tiles
{

	class Layers
	{
	public:
		Layers() = default;
		~Layers() = default;

		void AddLayer();
		void DeleteLayer();
		void RemoveLayer(size_t index = std::numeric_limits<size_t>::max());
		void ClearLayer(size_t index = std::numeric_limits<size_t>::max());
		void InsertLayer(Layer& layer);
		void InsertLayer(size_t index, Layer& layer);
		void ReplaceLayer(size_t index, Layer& layer);

		void ClearAllLayers() { m_Layers.clear(); }
		void Resize(size_t width, size_t height);
		bool IsEmpty() { return m_Layers.empty(); }

		// ActiveLayer
		void SetActiveLayer(size_t index) { m_ActiveLayer = index; }
		size_t GetActiveLayer() { return m_ActiveLayer; }

		// Getters
		size_t GetWidth() const { return m_LayerWidth; }
		size_t GetHeight() const { return m_LayerHeight; }
		size_t GetSize() { return m_Layers.size(); }
		
		Layer& GetLayer(size_t index) { return m_Layers[index]; }
		Tile& GetTile(size_t index, size_t y, size_t x) { return m_Layers[index].GetTile(y, x); }
	private:
		bool IsLayerInBounds(size_t index) const;
		size_t ResolveLayerIndex(size_t index) const;
	private:
		size_t m_LayerWidth = 0;		// Height of each layer
		size_t m_LayerHeight = 0;		// Width of each layer

		size_t m_ActiveLayer = 0;		// Working layer

		std::vector<Layer> m_Layers;	// Layers
	};

}