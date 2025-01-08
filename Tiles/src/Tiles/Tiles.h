#include <vector>
#include <iostream>

#include "imgui.h"

#include "Lumina/Layer.h"
#include "Lumina/Utils/Timer.h"

class Tiles : public Lumina::Layer
{
public:
    virtual void OnAttach() override
    {

    }

    virtual void OnDetach() override
    {

    }

    virtual void OnUpdate(float timestep) override
    {
        float elapsedTime = m_FrameTimer.Elapsed();
        m_FPS = 1.0f / elapsedTime;
        m_FrameTimer.Reset();
    }

    virtual void OnUIRender() override
    {
        ImGui::Begin("Example Window");
        ImGui::Text("Hello World!");
        ImGui::End();

        ImGui::Begin("FPS Counter");
        ImGui::Text("FPS: %.1f", m_FPS);
        ImGui::End();
    }

private:
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0.0f;
};