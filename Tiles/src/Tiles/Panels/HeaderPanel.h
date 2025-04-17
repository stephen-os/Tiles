#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "../Core/Layers.h"
#include "../Core/Exporter.h"
#include "../Core/TextureSelection.h"

#include "Lumina/Core/Aliases.h"

#include "../Commands/CommandHistory.h"

#include <string>

namespace Tiles
{

	class HeaderPanel
	{
	public:
		void OnUIRender();

		void SetTextureAtlas(const Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; }
		void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
		void SetTextureSelection(const Shared<TextureSelection>& textureSelection) { m_TextureSelection = textureSelection; }
		void SetCommandHistory(const Shared<CommandHistory>& commandHistory) { m_CommandHistory = commandHistory; }
	private:
		void RenderFile();
		void RenderEdit();
		void RenderExample();
		void RenderHelp();

		// Popups
		void RenderNewPopup();
		void RenderRenderMatrixPopup();
		void RenderAboutPopup();

	private:
		Shared<Layers> m_Layers;
		Shared<Lumina::TextureAtlas> m_Atlas;
		Shared<TextureSelection> m_TextureSelection;
		Shared<CommandHistory> m_CommandHistory;

		bool m_ShowNewPopup = false;
		bool m_ShowRenderMatrixPopup = false;
		bool m_ShowAboutPopup = false;

		int m_NewWidth = 10;
		int m_NewHeight = 10;

		std::vector<bool> m_Checkboxes;
		char m_ExportFileName[256] = "";
		char m_ExportFilePath[256] = "";
	};

}