#pragma once

#include <glm/glm.hpp>

#include "json.hpp"

#include <cstdint>

namespace Tiles
{
	// A single grid cell: paint/texture flags plus transform, tint, and the
	// atlas sub-region it samples. Default-constructs to an empty, unpainted tile.
	class Tile
	{
	public:
		Tile() = default;
		~Tile() = default;

		// Restores the tile to its default empty state.
		void Reset();

		[[nodiscard]] bool IsPainted() const { return m_IsPainted; }
		void SetPainted(bool painted) { m_IsPainted = painted; }

		[[nodiscard]] bool IsTextured() const { return m_IsTexture; }
		void SetTextured(bool textured) { m_IsTexture = textured; }

		[[nodiscard]] size_t GetAtlasIndex() const { return m_AtlasIndex; }
		void SetAtlasIndex(size_t index) { m_AtlasIndex = index; }
		[[nodiscard]] bool HasValidAtlas() const { return m_AtlasIndex != INVALID_ATLAS_INDEX; }

		[[nodiscard]] glm::vec3& GetRotation() { return m_Rotation; }
		[[nodiscard]] glm::vec2& GetSize() { return m_Size; }
		[[nodiscard]] glm::vec4& GetTint() { return m_TintColor; }
		[[nodiscard]] glm::vec4& GetTextureCoords() { return m_TextureCoords; }

		[[nodiscard]] const glm::vec3& GetRotation() const { return m_Rotation; }
		[[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }
		[[nodiscard]] const glm::vec4& GetTint() const { return m_TintColor; }
		[[nodiscard]] const glm::vec4& GetTextureCoords() const { return m_TextureCoords; }

		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
		void SetSize(const glm::vec2& size) { m_Size = size; }
		void SetTint(const glm::vec4& tint) { m_TintColor = tint; }
		void SetTextureCoords(const glm::vec4& textureCoords) { m_TextureCoords = textureCoords; }

		// Value equality; float fields are compared with an epsilon tolerance.
		[[nodiscard]] bool operator==(const Tile& other) const;
		[[nodiscard]] bool operator!=(const Tile& other) const;

		static constexpr size_t INVALID_ATLAS_INDEX = SIZE_MAX;

		[[nodiscard]] nlohmann::json ToJSON() const;
		[[nodiscard]] static Tile FromJSON(const nlohmann::json& j);

	private:
		bool m_IsPainted = false;
		bool m_IsTexture = false;

		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 1.0f, 1.0f };
		glm::vec4 m_TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		size_t m_AtlasIndex = INVALID_ATLAS_INDEX;
		glm::vec4 m_TextureCoords = { 0, 0, 1, 1 };
	};
}
