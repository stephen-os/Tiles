#include "Input.h"

#include "Application.h"
#include "InputState.h"

namespace Tiles::Input
{
	// Every query forwards to the application's event-fed InputState, so all input
	// -- shortcuts and held-state alike -- derives from Application::OnEvent.

	// True while the key is held this frame.
	bool IsKeyDown(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyDown(key);
	}

	// True only on the frame the key goes down.
	bool IsKeyPressed(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyPressed(key);
	}

	// True only on the frame the key comes up.
	bool IsKeyReleased(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyReleased(key);
	}

	// True while the mouse button is held this frame.
	bool IsMouseButtonDown(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonDown(button);
	}

	// True only on the frame the mouse button goes down.
	bool IsMouseButtonPressed(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonPressed(button);
	}

	// True only on the frame the mouse button comes up.
	bool IsMouseButtonReleased(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonReleased(button);
	}
}
