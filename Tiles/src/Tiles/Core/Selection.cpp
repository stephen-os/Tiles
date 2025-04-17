#include "Selection.h"

namespace Tiles
{
	static Selection::Mode s_CurrentMode = Selection::Mode::None;

	Selection::Mode Selection::GetCurrentMode()
	{
		return s_CurrentMode;
	}
	void Selection::SetCurrentMode(Selection::Mode mode)
	{
		s_CurrentMode = mode;
	}
}