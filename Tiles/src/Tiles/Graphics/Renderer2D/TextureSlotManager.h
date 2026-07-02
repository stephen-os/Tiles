#pragma once

#include "../Renderer2D.h"

#include <array>
#include <functional>
#include <memory>

namespace Tiles
{
	// Owns the per-batch texture sampler slots. Slot 0 is a 1x1 white texture and
	// is also the result for a null texture. When all slots are consumed the whole
	// renderer is flushed via a callback (installed by the facade) so the next
	// texture starts a fresh set of slots.
	class TextureSlotManager
	{
	public:
		void Init();
		void Shutdown();

		void SetFlushCallback(std::function<void()> flush) { m_Flush = std::move(flush); }

		// Resets to only slot 0 (white) being live for a new batch.
		void Reset() { Index = 1; }

		float ComputeTextureIndex(const std::shared_ptr<Texture>& texture);

		void Bind() const;
		void Unbind() const;

		std::array<std::shared_ptr<Texture>, MaxTextureSlots> Slots;
		uint32_t Index = 1;

	private:
		std::function<void()> m_Flush;
	};
}
