#pragma once

#include "../LayerStack.h"

namespace Tiles
{
	class Command
	{
	public:
		virtual ~Command() {}

		virtual void Execute(LayerStack& layerStack) = 0;
		virtual void Undo(LayerStack& layerStack) = 0;

		virtual bool Validate(const Command& other) const { return false; }
	};
}