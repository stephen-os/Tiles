#pragma once

#include "Layer.h"

namespace Tiles
{

	class Tools
	{
	public:
		static void Fill(Layer& layer, int newTextureIndex, size_t y, size_t x);
	};

}