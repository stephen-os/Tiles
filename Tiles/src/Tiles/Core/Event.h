#pragma once

#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <format>
#include <functional>

namespace Tiles
{
	// The event layer builds on the input-code types, which live in Tiles::Input;
	// surface them here so events can name KeyCode/KeyMods/MouseCode directly.
	using Input::KeyCode;
	using Input::KeyMods;
	using Input::MouseCode;

	// Every kind of window/input event the application can raise.
	enum class EventType
	{
		None = 0,

		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		KeyPressed,
		KeyReleased,
		KeyTyped,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
	};

	// Maps an EventType to its name for logging and display.
	constexpr std::string_view EventTypeToString(EventType type)
	{
		switch (type)
		{
		case EventType::None:					return "None";
		case EventType::WindowClose:			return "WindowClose";
		case EventType::WindowResize:			return "WindowResize";
		case EventType::WindowFocus:			return "WindowFocus";
		case EventType::WindowLostFocus:		return "WindowLostFocus";
		case EventType::WindowMoved:			return "WindowMoved";
		case EventType::KeyPressed:				return "KeyPressed";
		case EventType::KeyReleased:			return "KeyReleased";
		case EventType::KeyTyped:				return "KeyTyped";
		case EventType::MouseButtonPressed:		return "MouseButtonPressed";
		case EventType::MouseButtonReleased:	return "MouseButtonReleased";
		case EventType::MouseMoved:				return "MouseMoved";
		case EventType::MouseScrolled:			return "MouseScrolled";
		default:								return "Unknown";
		}
	}

	// Abstract base for every event. Events flow top-down through the Layer stack
	// and are consumed by reference; a handler calls MarkHandled to stop
	// propagation. Held only by reference, so copy/move are deleted to prevent
	// slicing.
	class Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;

		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;
		Event(Event&&) = delete;
		Event& operator=(Event&&) = delete;

		// The concrete type of this event.
		[[nodiscard]] virtual EventType GetType() const = 0;

		// The event's name, matching its EventType.
		[[nodiscard]] virtual std::string_view GetName() const = 0;

		// A human-readable form for logging; defaults to the name.
		[[nodiscard]] virtual std::string ToString() const { return std::string(GetName()); }

		// True once a handler has claimed the event.
		[[nodiscard]] bool IsHandled() const { return m_Handled; }

		// Claims the event so it stops propagating to lower layers.
		void MarkHandled() { m_Handled = true; }

	private:
		bool m_Handled = false;
	};

	// Wires an event's compile-time EventType into the virtual interface, so a
	// concrete event only has to declare its payload.
	template<EventType Type>
	class EventBase : public Event
	{
	public:
		// The event's type as a compile-time constant, used by EventDispatcher.
		[[nodiscard]] static constexpr EventType StaticType() { return Type; }

		// The concrete type of this event.
		[[nodiscard]] EventType GetType() const override { return Type; }

		// The event's name, matching its EventType.
		[[nodiscard]] std::string_view GetName() const override { return EventTypeToString(Type); }
	};

	// Shared payload for key press/release events: the key and active modifiers.
	template<EventType Type>
	class KeyEventBase : public EventBase<Type>
	{
	public:
		KeyEventBase(KeyCode key, KeyMods mods = KeyMods::None) : m_Key(key), m_Mods(mods) {}

		// The key this event concerns.
		[[nodiscard]] KeyCode GetKey() const { return m_Key; }

		// The modifier keys held when the event fired.
		[[nodiscard]] KeyMods GetMods() const { return m_Mods; }

		// True if the given modifier was held.
		[[nodiscard]] bool HasMod(KeyMods mod) const { return Input::HasMod(m_Mods, mod); }

		// Formats the event and its key code for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}", this->GetName(), static_cast<uint16_t>(m_Key));
		}

	protected:
		KeyCode m_Key;
		KeyMods m_Mods;
	};

	// Shared payload for mouse button press/release events: the button and modifiers.
	template<EventType Type>
	class MouseButtonEventBase : public EventBase<Type>
	{
	public:
		MouseButtonEventBase(MouseCode button, KeyMods mods = KeyMods::None) : m_Button(button), m_Mods(mods) {}

		// The mouse button this event concerns.
		[[nodiscard]] MouseCode GetButton() const { return m_Button; }

		// The modifier keys held when the event fired.
		[[nodiscard]] KeyMods GetMods() const { return m_Mods; }

		// True if the given modifier was held.
		[[nodiscard]] bool HasMod(KeyMods mod) const { return Input::HasMod(m_Mods, mod); }

		// Formats the event and its button code for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}", this->GetName(), static_cast<uint16_t>(m_Button));
		}

	protected:
		MouseCode m_Button;
		KeyMods m_Mods;
	};

	// Window events

	// The user has requested the window be closed.
	class WindowCloseEvent final : public EventBase<EventType::WindowClose>
	{
	public:
		WindowCloseEvent() = default;
	};

	// The window's framebuffer was resized.
	class WindowResizeEvent final : public EventBase<EventType::WindowResize>
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		// The new framebuffer width in pixels.
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }

		// The new framebuffer height in pixels.
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }

		// Formats the event and new size for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}x{}", GetName(), m_Width, m_Height);
		}

	private:
		uint32_t m_Width, m_Height;
	};

	// The window gained input focus.
	class WindowFocusEvent final : public EventBase<EventType::WindowFocus>
	{
	public:
		WindowFocusEvent() = default;
	};

	// The window lost input focus.
	class WindowLostFocusEvent final : public EventBase<EventType::WindowLostFocus>
	{
	public:
		WindowLostFocusEvent() = default;
	};

	// The window's top-left moved to a new screen position.
	class WindowMovedEvent final : public EventBase<EventType::WindowMoved>
	{
	public:
		WindowMovedEvent(int32_t x, int32_t y) : m_X(x), m_Y(y) {}

		// The new x position in screen coordinates.
		[[nodiscard]] int32_t GetX() const { return m_X; }

		// The new y position in screen coordinates.
		[[nodiscard]] int32_t GetY() const { return m_Y; }

		// Formats the event and new position for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_X, m_Y);
		}

	private:
		int32_t m_X, m_Y;
	};

	// Keyboard events

	// A key was pressed; IsRepeat marks OS auto-repeat while the key is held.
	class KeyPressedEvent final : public KeyEventBase<EventType::KeyPressed>
	{
	public:
		KeyPressedEvent(KeyCode key, KeyMods mods = KeyMods::None, bool isRepeat = false)
			: KeyEventBase(key, mods), m_IsRepeat(isRepeat) {}

		// True when this press is an OS auto-repeat rather than the initial press.
		[[nodiscard]] bool IsRepeat() const { return m_IsRepeat; }

		// Formats the event, key code, and repeat flag for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}{}", GetName(), static_cast<uint16_t>(m_Key), m_IsRepeat ? " (repeat)" : "");
		}

	private:
		bool m_IsRepeat;
	};

	// A key was released.
	class KeyReleasedEvent final : public KeyEventBase<EventType::KeyReleased>
	{
	public:
		KeyReleasedEvent(KeyCode key, KeyMods mods = KeyMods::None) : KeyEventBase(key, mods) {}
	};

	// A character was typed (a Unicode code point from text input).
	class KeyTypedEvent final : public EventBase<EventType::KeyTyped>
	{
	public:
		KeyTypedEvent(uint32_t character) : m_Character(character) {}

		// The typed Unicode code point.
		[[nodiscard]] uint32_t GetCharacter() const { return m_Character; }

		// Formats the event and code point for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}", GetName(), m_Character);
		}

	private:
		uint32_t m_Character;
	};

	// Mouse events

	// A mouse button was pressed.
	class MouseButtonPressedEvent final : public MouseButtonEventBase<EventType::MouseButtonPressed>
	{
	public:
		MouseButtonPressedEvent(MouseCode button, KeyMods mods = KeyMods::None) : MouseButtonEventBase(button, mods) {}
	};

	// A mouse button was released.
	class MouseButtonReleasedEvent final : public MouseButtonEventBase<EventType::MouseButtonReleased>
	{
	public:
		MouseButtonReleasedEvent(MouseCode button, KeyMods mods = KeyMods::None) : MouseButtonEventBase(button, mods) {}
	};

	// The mouse moved to a new position in the window.
	class MouseMovedEvent final : public EventBase<EventType::MouseMoved>
	{
	public:
		MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

		// The new cursor x position in window coordinates.
		[[nodiscard]] float GetX() const { return m_X; }

		// The new cursor y position in window coordinates.
		[[nodiscard]] float GetY() const { return m_Y; }

		// Formats the event and position for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_X, m_Y);
		}

	private:
		float m_X, m_Y;
	};

	// The mouse wheel scrolled by the given per-axis offsets.
	class MouseScrolledEvent final : public EventBase<EventType::MouseScrolled>
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		// The horizontal scroll offset.
		[[nodiscard]] float GetXOffset() const { return m_XOffset; }

		// The vertical scroll offset.
		[[nodiscard]] float GetYOffset() const { return m_YOffset; }

		// Formats the event and offsets for logging.
		[[nodiscard]] std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_XOffset, m_YOffset);
		}

	private:
		float m_XOffset, m_YOffset;
	};

	// Dispatch

	// Constrains EventDispatcher to Event-derived types.
	template<typename T>
	concept IsEvent = std::is_base_of_v<Event, T>;

	// Routes an event to a type-matched handler. Held by reference for the
	// duration of one OnEvent pass.
	class EventDispatcher
	{
	public:
		explicit EventDispatcher(Event& e) : m_Event(e) {}

		// Invokes func with the event as T& iff its runtime type is T; returns
		// whether the type matched. A true return from func marks the event handled.
		template<IsEvent T, typename F>
		requires std::invocable<F, T&>
		bool Dispatch(F&& func)
		{
			if (m_Event.GetType() == T::StaticType())
			{
				bool handled = std::invoke(std::forward<F>(func), static_cast<T&>(m_Event));
				if (handled)
					m_Event.MarkHandled();
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};
}
