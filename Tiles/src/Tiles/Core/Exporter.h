#pragma once

#include "Layers.h"
#include "Atlas.h"

#include "Base.h"

namespace Tiles
{

	class Exporter
	{
	public:
		int& GetResolution() { return m_Resolution; }
		void Export(Shared<Layers>& layers, Shared<Atlas>& atlas, std::string& filename);
	private:
		int m_Resolution = 64;
	};

}