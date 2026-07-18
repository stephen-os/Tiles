#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace Tiles { class Texture; class TextureAtlas; }

namespace Tiles::Editor
{
	// Caches the GL texture for each Domain TextureAtlas, decoding its image bytes
	// and uploading lazily. This is the one place the pure-data atlas meets the
	// graphics layer; an entry is rebuilt when the atlas's image version changes.
	class AtlasTextureCache
	{
	public:
		// The GL texture for an atlas, built on first use and rebuilt when the
		// atlas's image changes. Null if the atlas has no image or decoding fails.
		[[nodiscard]] std::shared_ptr<Tiles::Texture> Get(const Tiles::TextureAtlas& atlas);

		// Drops all cached textures (e.g. when the active project changes).
		void Clear();

		// Drops the cached texture for one atlas -- called when its document closes,
		// so an entry never outlives the atlas it was built from.
		void Evict(const Tiles::TextureAtlas& atlas) { m_Entries.erase(&atlas); }

	private:
		struct Entry
		{
			std::shared_ptr<Tiles::Texture> Texture;
			uint64_t Version = 0;
		};

		std::unordered_map<const Tiles::TextureAtlas*, Entry> m_Entries;
	};
}
