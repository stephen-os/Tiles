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

		void RemoveLayer(size_t index = std::numeric_limits<size_t>::max());
		void ClearLayer(size_t index = std::numeric_limits<size_t>::max());
		void InsertLayer(Layer& layer);
		void InsertLayer(size_t index, Layer& layer);
		void ReplaceLayer(size_t index, Layer& layer);
		void ClearAllLayers() { m_Layers.clear(); }

		void ShiftLayerUp();
		void ShiftLayerDown();

		void Resize(size_t width, size_t height);
		bool IsEmpty() const { return m_Layers.empty(); }

		// Setters
		void SetActiveLayer(size_t index) { m_ActiveLayer = index; }
		void SetName(const std::string name) { m_Name = name; }

		// Getters
		size_t GetActiveLayer() const { return m_ActiveLayer; }
		size_t GetWidth() const { return m_LayerWidth; }
		size_t GetHeight() const { return m_LayerHeight; }
		size_t GetSize() const { return m_Layers.size(); }
		const std::string GetName() const { return m_Name; }
		
		Layer& GetLayer(size_t index) { return m_Layers[index]; }
		const Layer& GetLayer(size_t index) const { return m_Layers[index]; }

		Tile& GetTile(size_t index, size_t y, size_t x) { return m_Layers[index].GetTile(y, x); }
		const Tile& GetTile(size_t index, size_t y, size_t x) const { return m_Layers[index].GetTile(y, x); }

	private:
		size_t ResolveLayerIndex(size_t index) const;
	private:
		std::string m_Name = "New Project"; // Name of the project
		size_t m_LayerWidth = 0;			// Height of each layer
		size_t m_LayerHeight = 0;			// Width of each layer
		size_t m_ActiveLayer = 0;			// Working layer
		std::vector<Layer> m_Layers;		// Layers
	};
}