#pragma once

#include "../Core/Layers.h"

namespace Tiles
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(Layers& layers) = 0;
		virtual void Undo(Layers& layers) = 0;

		virtual bool Validate(const Command& other) const { return false; }
	};
}