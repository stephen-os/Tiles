#pragma once

#include "../Core/Layers.h"
#include "../Core/Exporter.h"
#include "../Core/Atlas.h"
#include "../Core/State.h"

#include "../Core/Base.h"

#include <string>

namespace Tiles
{

	class HeaderPanel
	{
	public:
		void OnUIRender();

		void SetTextureAtlas(const Shared<Atlas>& atlas) { m_Atlas = atlas; }
		void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
		void SetState(const Shared<State>& state) { m_State = state; }
	private:
		void RenderFile();
		void RenderEdit();
		void RenderOptions();

		void RenderNewPopup();
		void RenderRenderMatrixPopup();

	private:
		Shared<Layers> m_Layers;
		Shared<Atlas> m_Atlas;
		Shared<State> m_State;

		bool m_ShowNewPopup = false;
		bool m_ShowRenderMatrixPopup = false;

		int m_NewWidth = 10;
		int m_NewHeight = 10;

		std::vector<bool> m_Checkboxes;
		char m_ExportFileName[256] = "";
		char m_ExportFilePath[256] = "";
	};

}