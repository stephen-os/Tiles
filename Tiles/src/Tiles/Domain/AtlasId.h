#pragma once

#include <cstdint>

namespace Tiles
{
	// A stable, per-project identity for a TextureAtlas. Assigned once at creation
	// from a monotonic counter and never reused, so tile references survive atlas
	// add / remove / reorder (unlike a positional index). Zero is the invalid id.
	enum class AtlasId : uint32_t
	{
		Invalid = 0
	};
}
