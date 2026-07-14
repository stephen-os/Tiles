#include "AtlasTextureCache.h"

#include "Domain/TextureAtlas.h"
#include "Graphics/Texture.h"

namespace Tiles::Editor
{
	// Returns the atlas's GL texture, rebuilding when its image version changes.
	std::shared_ptr<Tiles::Texture> AtlasTextureCache::Get(const Tiles::TextureAtlas& atlas)
	{
		if (!atlas.HasImage())
			return nullptr;

		auto it = m_Entries.find(&atlas);
		if (it != m_Entries.end() && it->second.Version == atlas.GetVersion())
			return it->second.Texture;

		// Decode the atlas's encoded image bytes and upload a fresh texture. A null
		// result is cached too, so a bad image is not re-decoded every frame.
		const std::vector<uint8_t>& bytes = atlas.GetImageBytes();
		auto texture = Tiles::Texture::CreateFromEncodedImage(bytes.data(), bytes.size());

		m_Entries[&atlas] = { texture, atlas.GetVersion() };
		return texture;
	}

	// Drops all cached textures.
	void AtlasTextureCache::Clear()
	{
		m_Entries.clear();
	}
}
