#include "RendererState.h"

namespace Tiles
{
	void TextureSlotManager::Init()
	{
		uint32_t whiteTextureData = 0xffffffff;
		Slots[0] = Texture::Create(1, 1);
		Slots[0]->SetData(&whiteTextureData, sizeof(uint32_t));
	}

	void TextureSlotManager::Shutdown()
	{
		for (auto& texture : Slots)
			texture.reset();
	}

	float TextureSlotManager::ComputeTextureIndex(const std::shared_ptr<Texture>& texture)
	{
		if (texture == nullptr)
			return 0.0f;

		for (uint32_t i = 1; i < Index; i++)
		{
			if (Slots[i] == texture)
				return static_cast<float>(i);
		}

		if (Index >= MaxTextureSlots)
			m_Flush();

		TILES_ASSERT(Index < MaxTextureSlots, "Texture slot index overflow!");

		float texIndex = static_cast<float>(Index);
		Slots[Index] = texture;
		Index++;

		return texIndex;
	}

	void TextureSlotManager::Bind() const
	{
		for (uint32_t i = 0; i < Index; i++)
			Slots[i]->Bind(i);
	}

	void TextureSlotManager::Unbind() const
	{
		for (uint32_t i = 0; i < Index; i++)
			Slots[i]->Unbind();
	}
}
