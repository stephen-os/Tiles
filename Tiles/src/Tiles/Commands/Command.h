#pragma once

#include "Domain/LayerStack.h"

namespace Tiles
{
	/// A reversible edit to a LayerStack, tracked by CommandHistory.
	/// Implementations capture the state needed to undo on their first Execute.
	class Command
	{
	public:
		virtual ~Command() {}

		virtual void Execute(LayerStack& layerStack) = 0;
		virtual void Undo(LayerStack& layerStack) = 0;

		/// @return True if other is an equivalent, redundant edit to this one, so
		///         the history can drop it. Defaults to false (never coalesce).
		virtual bool Validate(const Command& other) const { return false; }
	};
}