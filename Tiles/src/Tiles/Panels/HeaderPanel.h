#pragma once

#include "Lumina/Renderer/TextureAtlas.h"

#include "../Core/Layers.h"
#include "../Core/TileRenderer.h"

#include "Lumina/Core/Aliases.h"

#include "../Commands/CommandHistory.h"

namespace Tiles
{
	class HeaderPanel
	{
	public:
		void Render();

		void SetTextureAtlas(const Shared<Lumina::TextureAtlas>& atlas) { m_Atlas = atlas; }
		void SetLayers(const Shared<Layers>& layers) { m_Layers = layers; }
		void SetCommandHistory(const Shared<CommandHistory>& commandHistory) { m_CommandHistory = commandHistory; }
	private:
		void RenderFile();
		void RenderEdit();
		void RenderExample();
		void RenderHelp();

		void HandleInput();

		// Popups
		void RenderNewPopup();
		void RenderRenderMatrixPopup();
		void RenderAboutPopup();

	private:
		Shared<Layers> m_Layers;
		Shared<Lumina::TextureAtlas> m_Atlas;
		Shared<CommandHistory> m_CommandHistory;

		bool m_ShowNewPopup = false;
		bool m_ShowRenderMatrixPopup = false;
		bool m_ShowAboutPopup = false;

		int m_NewWidth = 10;
		int m_NewHeight = 10;

		ExportAttributes m_ExportAttributes;
		float m_ExportMessageTimer = 0.0f;
	};
}