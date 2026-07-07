#include "Input.h"

#include "Application.h"
#include "Window.h"

namespace Tiles::Input
{
    bool IsKeyPressed(KeyCode key)
    {
        return Application::GetInstance().GetWindow().IsKeyPressed(key);
    }

    bool IsMouseButtonPressed(MouseCode button)
    {
        return Application::GetInstance().GetWindow().IsMouseButtonPressed(button);
    }
}
