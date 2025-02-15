#pragma once

#include <stack>

#include "Layers.h"
#include "Layer.h"
#include "Tile.h"
#include "Base.h"

namespace Tiles
{

	enum class StateType
	{
		Layer_Insert,
		Layer_Replace,
		Layer_Delete,
		Tile_Replace
	};

	struct Action
	{
		StateType Type;

		// Layer
		size_t Index;
		Layer Layer;

		// Tile
		size_t Y;
		size_t X;
		Tile Tile;
	};

	class State
	{
	public:
		State() = default;
		~State() = default;

		void SetTileLayers(Shared<Layers> layers) { m_Layers = layers; }

		void PushLayer(size_t index, Layer& layer, StateType type = StateType::Layer_Replace);
		void PushTile(size_t y, size_t x, Tile& tile);

		void Undo();
		void Redo();
	private:
		void Trim();
	private:
		Shared<Layers> m_Layers;

		std::stack<Action> m_UndoStack;
		std::stack<Action> m_RedoStack;
		static constexpr size_t MAX_STACK = 50;
	};

}