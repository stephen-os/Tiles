#pragma once
#include "Panel.h"
#include "Lumina/Graphics/Texture.h"
#include <glm/glm.hpp>

using namespace Lumina;

namespace Tiles
{
    class PanelToolSelection : public Panel
    {
    public:
        PanelToolSelection();
        ~PanelToolSelection() = default;
        
        void Render() override;
		void Update() override;

    private:
        // Rendering methods
        void RenderToolButtons();
        void RenderToolButton(const char* id, const Ref<Lumina::Texture>& texture, PaintingMode mode);
        void RenderCustomCursor();
        void DrawCursorForMode(PaintingMode mode, const Ref<Lumina::Texture>& texture);

	private:
        // UI state
        Ref<Texture> m_BrushTexture = nullptr;
        Ref<Texture> m_EraserTexture = nullptr;
        Ref<Texture> m_FillTexture = nullptr;
        glm::vec2 m_ButtonSize = { 32.0f, 32.0f };
    };
}