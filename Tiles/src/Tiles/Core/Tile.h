#pragma once

#include "glm/glm.hpp"

// This is wrong we need nlohmann in the dependencies. 
#include "json.hpp"

namespace Tiles
{
	class Tile
	{
	public:
		Tile() = default;
		~Tile() = default;

		void Reset();

		bool IsPainted() const { return m_IsPainted; }
		void SetPainted(bool painted) { m_IsPainted = painted; }

		bool IsTextured() const { return m_IsTexture; }
		void SetTextured(bool textured) { m_IsTexture = textured; }

		size_t GetAtlasIndex() const { return m_AtlasIndex; }
		void SetAtlasIndex(size_t index) { m_AtlasIndex = index; }
		bool HasValidAtlas() const { return m_AtlasIndex != INVALID_ATLAS_INDEX; }

		glm::vec3& GetRotation() { return m_Rotation; };
		glm::vec2& GetSize() { return m_Size; };
		glm::vec4& GetTint() { return TintColor; };
		glm::vec4& GetTextureCoords() { return m_TextureCoords; };

		const glm::vec3& GetRotation() const { return m_Rotation; }
		const glm::vec2& GetSize() const { return m_Size; }
		const glm::vec4& GetTint() const { return TintColor; }
		const glm::vec4& GetTextureCoords() const { return m_TextureCoords; }

		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
		void SetSize(const glm::vec2& size) { m_Size = size; }
		void SetTint(const glm::vec4& tint) { TintColor = tint; }
		void SetTextureCoords(const glm::vec4& textureCoords) { m_TextureCoords = textureCoords; }

		bool operator==(const Tile& other) const;
		bool operator!=(const Tile& other) const;

		static constexpr size_t INVALID_ATLAS_INDEX = SIZE_MAX;

		nlohmann::json ToJSON() const;
		static Tile FromJSON(const nlohmann::json& j);

	private:
		bool m_IsPainted = false;
		bool m_IsTexture = false;

		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 1.0f, 1.0f };
		glm::vec4 TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		size_t m_AtlasIndex = INVALID_ATLAS_INDEX;
		glm::vec4 m_TextureCoords = { 0, 0, 1, 1 };
	};
}