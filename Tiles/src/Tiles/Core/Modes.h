#pragma once

namespace Tiles
{
	class Modes
	{
	public:
		enum class Mode
		{
			None = 0,
			Paint,
			Erase,
			Fill
		};

		static Mode GetCurrentMode();
		static void SetCurrentMode(Mode mode);
	};
}