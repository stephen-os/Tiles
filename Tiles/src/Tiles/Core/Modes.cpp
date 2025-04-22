#include "Modes.h"

namespace Tiles
{
	static Modes::Mode s_CurrentMode = Modes::Mode::None;

	Modes::Mode Modes::GetCurrentMode()
	{
		return s_CurrentMode;
	}
	void Modes::SetCurrentMode(Modes::Mode mode)
	{
		s_CurrentMode = mode;
	}
}