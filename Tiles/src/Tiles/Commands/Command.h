#pragma once

#include "../Core/Layers.h"

namespace Tiles
{
	struct Position
	{
		int index; 
		int x;
		int y;
	};

	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(Layers& layers) = 0;
		virtual void Undo(Layers& layers) = 0;
	};
}