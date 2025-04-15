#pragma once

#include "../Core/Layers.h"

namespace Tiles
{
	struct Position
	{
		int L; 
		int Y;
		int X;

		bool operator==(const Position& other) const
		{
			return L == other.L && Y == other.Y && X == other.X;
		}
	};

	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(Layers& layers) = 0;
		virtual void Undo(Layers& layers) = 0;

		virtual bool Validate(const Command& other) const { return false; }
	};
}