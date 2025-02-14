#pragma once

#include "../Core/Layers.h"
#include "../Core/Exporter.h"
#include "../Core/Atlas.h"
#include "../Core/State.h"

#include "Lumina/Base.h"

class HeaderPanel
{
public:
	void Render();

	void SetTextureAtlas(const Lumina::Ref<Atlas>& atlas) { m_Atlas = atlas; }
	void SetTileLayers(const Lumina::Ref<Layers>& layers) { m_Layers = layers; }
	void SetState(const Lumina::Ref<State>& state) { m_State = state; }
private:
	void RenderFile();
	void RenderEdit();
	void RenderOptions(); 

	void RenderNewPopup();

private:
	Lumina::Ref<Layers> m_Layers;
	Lumina::Ref<Atlas> m_Atlas;
	Lumina::Ref<State> m_State;

	// New
	bool m_ShowNewPopup = false;
	int m_NewWidth = 10;
	int m_NewHeight = 10;
};