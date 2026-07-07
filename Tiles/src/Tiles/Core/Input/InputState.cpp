#include "InputState.h"

namespace Tiles::Input
{
	namespace Utils
	{
		// Advances one slot from its edge to its steady state: Pressed -> Held,
		// Released -> None. Held and None are left unchanged.
		inline void Advance(KeyState& state)
		{
			if (state == KeyState::Pressed)
				state = KeyState::Held;
			else if (state == KeyState::Released)
				state = KeyState::None;
		}

		// True when a slot counts as down this frame (Pressed or Held).
		inline bool IsDown(KeyState state)
		{
			return state == KeyState::Pressed || state == KeyState::Held;
		}
	}

	// Advances every key and button edge into its steady state for the new frame.
	void InputState::NewFrame()
	{
		for (KeyState& state : m_Keys)
			Utils::Advance(state);
		for (KeyState& state : m_MouseButtons)
			Utils::Advance(state);
	}

	// Records a key press as a fresh edge; repeats are ignored so the key stays Held.
	void InputState::OnKeyPressed(KeyCode key, bool isRepeat)
	{
		if (isRepeat)
			return;

		auto index = static_cast<size_t>(key);
		if (index < KeyCount)
			m_Keys[index] = KeyState::Pressed;
	}

	// Records a key release edge.
	void InputState::OnKeyReleased(KeyCode key)
	{
		auto index = static_cast<size_t>(key);
		if (index < KeyCount)
			m_Keys[index] = KeyState::Released;
	}

	// Records a mouse button press edge.
	void InputState::OnMouseButtonPressed(MouseCode button)
	{
		auto index = static_cast<size_t>(button);
		if (index < MouseButtonCount)
			m_MouseButtons[index] = KeyState::Pressed;
	}

	// Records a mouse button release edge.
	void InputState::OnMouseButtonReleased(MouseCode button)
	{
		auto index = static_cast<size_t>(button);
		if (index < MouseButtonCount)
			m_MouseButtons[index] = KeyState::Released;
	}

	// Clears all state to None so keys held when focus left don't get stuck down.
	void InputState::Reset()
	{
		m_Keys.fill(KeyState::None);
		m_MouseButtons.fill(KeyState::None);
	}

	// True while the key is held this frame (Pressed or Held).
	bool InputState::IsKeyDown(KeyCode key) const
	{
		auto index = static_cast<size_t>(key);
		return index < KeyCount && Utils::IsDown(m_Keys[index]);
	}

	// True only on the frame the key went down.
	bool InputState::IsKeyPressed(KeyCode key) const
	{
		auto index = static_cast<size_t>(key);
		return index < KeyCount && m_Keys[index] == KeyState::Pressed;
	}

	// True only on the frame the key came up.
	bool InputState::IsKeyReleased(KeyCode key) const
	{
		auto index = static_cast<size_t>(key);
		return index < KeyCount && m_Keys[index] == KeyState::Released;
	}

	// True while the mouse button is held this frame (Pressed or Held).
	bool InputState::IsMouseButtonDown(MouseCode button) const
	{
		auto index = static_cast<size_t>(button);
		return index < MouseButtonCount && Utils::IsDown(m_MouseButtons[index]);
	}

	// True only on the frame the mouse button went down.
	bool InputState::IsMouseButtonPressed(MouseCode button) const
	{
		auto index = static_cast<size_t>(button);
		return index < MouseButtonCount && m_MouseButtons[index] == KeyState::Pressed;
	}

	// True only on the frame the mouse button came up.
	bool InputState::IsMouseButtonReleased(MouseCode button) const
	{
		auto index = static_cast<size_t>(button);
		return index < MouseButtonCount && m_MouseButtons[index] == KeyState::Released;
	}
}
