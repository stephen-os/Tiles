#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Tiles::Input
{
	// Live queries over the event-fed InputState -- the single source of input
	// truth, updated in Application::OnEvent. Down = held this frame; Pressed and
	// Released report the down/up edge on the frame it happens.

	// True while the key is held this frame.
	bool IsKeyDown(KeyCode key);
	// True only on the frame the key goes down.
	bool IsKeyPressed(KeyCode key);
	// True only on the frame the key comes up.
	bool IsKeyReleased(KeyCode key);

	// True while the mouse button is held this frame.
	bool IsMouseButtonDown(MouseCode button);
	// True only on the frame the mouse button goes down.
	bool IsMouseButtonPressed(MouseCode button);
	// True only on the frame the mouse button comes up.
	bool IsMouseButtonReleased(MouseCode button);
}
