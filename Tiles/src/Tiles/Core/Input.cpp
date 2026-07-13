#include "Input.h"

#include "Application.h"
#include "InputState.h"

namespace Tiles::Input
{
	// Every query forwards to the application's event-fed InputState, so all input
	// -- shortcuts and held-state alike -- derives from Application::OnEvent.

	bool IsKeyDown(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyDown(key);
	}

	bool IsKeyPressed(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyPressed(key);
	}

	bool IsKeyReleased(KeyCode key)
	{
		return Application::GetInstance().GetInputState().IsKeyReleased(key);
	}

	bool IsMouseButtonDown(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonDown(button);
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonPressed(button);
	}

	bool IsMouseButtonReleased(MouseCode button)
	{
		return Application::GetInstance().GetInputState().IsMouseButtonReleased(button);
	}
}
