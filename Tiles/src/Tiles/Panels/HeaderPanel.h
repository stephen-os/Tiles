#pragma once

#include "../Core/Layers.h"
#include "../Core/Exporter.h"
#include "../Core/Atlas.h"
#include "../Core/State.h"

#include "../Core/Base.h"

namespace Tiles
{

	class HeaderPanel
	{
	public:
		void Render();

		void SetTextureAtlas(const Shared<Atlas>& atlas) { m_Atlas = atlas; }
		void SetTileLayers(const Shared<Layers>& layers) { m_Layers = layers; }
		void SetState(const Shared<State>& state) { m_State = state; }
	private:
		void RenderFile();
		void RenderEdit();
		void RenderOptions();

		void RenderNewPopup();

	private:
		Shared<Layers> m_Layers;
		Shared<Atlas> m_Atlas;
		Shared<State> m_State;

		// New
		bool m_ShowNewPopup = false;
		int m_NewWidth = 10;
		int m_NewHeight = 10;
	};

}