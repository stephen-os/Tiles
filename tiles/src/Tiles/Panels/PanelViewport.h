#pragma once

#include "Panel.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"

#include "Lumina/Graphics/RenderTarget.h"
#include "Lumina/Graphics/Texture.h"

#include "Lumina/Graphics/Renderer2D.h"

#include "imgui.h"

using namespace Lumina;

namespace Tiles
{
    class PanelViewport : public Panel
    {
    public:
        PanelViewport();
        ~PanelViewport() = default;

        void Render() override;
        void Update() override; 

    private:
        glm::vec2 ScreenToWorld() const;
        ImVec2 WorldToScreen() const;

    private: 
        Lumina::OrthographicCamera m_Camera;
        Ref<RenderTarget> m_RenderTarget;
        float m_TileSize;
		float m_MouseDelta = 0.0f;

        // Demo quad state
        QuadAttributes m_DemoQuad;
        QuadAttributes m_MouseFollowQuad; // For debug. 

		ImVec2 m_MousePosition = { 0.0f, 0.0f };
        ImVec2 m_ViewportPosition = { 0.0f, 0.0f };
		ImVec2 m_ViewportSize = { 512.0f, 512.0f };
    };
}