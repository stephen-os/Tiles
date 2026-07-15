#pragma once

#include "Domain/LayerStack.h"

namespace Tiles
{
	// A reversible edit to a LayerStack, tracked by CommandHistory.
	// Implementations capture the state needed to undo on their first Execute.
	class Command
	{
	public:
		virtual ~Command() = default;

		virtual void Execute(LayerStack& layerStack) = 0;
		virtual void Undo(LayerStack& layerStack) = 0;

		// @return True if other is a redundant repeat of this command, so the
		//         history can coalesce it (drop the new one). Defaults to false.
		[[nodiscard]] virtual bool CanCoalesce(const Command& other) const { return false; }
	};
}
