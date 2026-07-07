#pragma once

#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"

namespace Tiles::Input
{
    // Polls whether a key is currently held. State comes from the window (GLFW),
    // so this reflects the live keyboard, not the buffered event stream.
    bool IsKeyPressed(KeyCode key);

    // Polls whether a mouse button is currently held.
    bool IsMouseButtonPressed(MouseCode button);
}
