#pragma once

#include "Layers.h"
#include "Atlas.h"

#include "Lumina/Base.h"

class Exporter
{
public:
	int& GetResolution() { return m_Resolution; }
	void Export(Lumina::Ref<Layers>& layers, Lumina::Ref<Atlas>& atlas, std::string& filename);
private:
	int m_Resolution = 64;
};