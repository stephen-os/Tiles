#pragma once

#include <vector>
#include <memory>

#include "Core/Layer.h"
#include "Utils/Timer.h"

#include "Panels/PanelManager.h"

namespace Tiles
{
    class Editor : public Tiles::Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float timestep) override;
        virtual void OnUIRender() override;

    private:
        PanelManager m_PanelManager;

        Tiles::Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}