#pragma once

#include <array>
#include <cstddef>

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Tiles::Input
{
	// Per-key and per-button state tracked across frames and fed by the event
	// stream. Events set the momentary Pressed/Released edges; NewFrame() advances
	// those into the steady Held/None states. This lets callers ask both "is it
	// held right now" and "did it go down this frame" -- and combine queries for
	// multi-key combos -- without polling the device directly.
	class InputState
	{
	public:
		// Advance edges into steady state: Pressed -> Held, Released -> None.
		// Call once per frame, before the frame's events are pumped in.
		void NewFrame();

		// Event feed. Call before events are dispatched to consumers, so a consumed
		// event still updates state.
		void OnKeyPressed(KeyCode key, bool isRepeat);
		void OnKeyReleased(KeyCode key);
		void OnMouseButtonPressed(MouseCode button);
		void OnMouseButtonReleased(MouseCode button);

		// Clears everything to None. Use on focus loss so keys held when focus left
		// don't get stuck "down" (their release event never arrives).
		void Reset();

		// Down = held this frame (Pressed or Held). Pressed = went down this frame
		// only. Released = came up this frame.
		[[nodiscard]] bool IsKeyDown(KeyCode key) const;
		[[nodiscard]] bool IsKeyPressed(KeyCode key) const;
		[[nodiscard]] bool IsKeyReleased(KeyCode key) const;

		[[nodiscard]] bool IsMouseButtonDown(MouseCode button) const;
		[[nodiscard]] bool IsMouseButtonPressed(MouseCode button) const;
		[[nodiscard]] bool IsMouseButtonReleased(MouseCode button) const;

	private:
		// GLFW's key range tops out at 348 (Menu); size for headroom and guard the
		// stray out-of-range codes (e.g. GLFW_KEY_UNKNOWN maps to a huge value).
		static constexpr size_t KeyCount = 512;
		static constexpr size_t MouseButtonCount = 8;

		std::array<KeyState, KeyCount> m_Keys{};
		std::array<KeyState, MouseButtonCount> m_MouseButtons{};
	};
}
