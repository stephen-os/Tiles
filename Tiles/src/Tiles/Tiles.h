#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Layer.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/FileReader.h"

#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/FrameBuffer.h"
#include "Lumina/Renderer/Texture.h"
#include "Lumina/Renderer/VertexArray.h"

// Client
#include "TileEditor.h"
#include "theme/ThemeManager.h"

class Tiles : public Lumina::Layer
{
public:
    virtual void OnUpdate(float timestep) override
    {
        float elapsedTime = m_FrameTimer.Elapsed();
        m_FPS = 1.0f / elapsedTime;
        m_FrameTimer.Reset();
    }

    virtual void OnUIRender() override
    {
        m_TileEditor.Render(); 

        ImGui::Begin("FPS Counter");
        ImGui::Text("FPS: %.1f", m_FPS);
        ImGui::End(); 
    }

    virtual void OnAttach() override
    {
        ThemeManager::GetInstance().ApplyDarkTheme();
        m_TileEditor.Init();
    }

    virtual void OnDetach() override
    {
        m_TileEditor.Shutdown();
    }
private:
    TileEditor m_TileEditor; 

    Lumina::Timer m_FrameTimer;
    float m_FPS = 0;
};