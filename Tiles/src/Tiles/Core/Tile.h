#pragma once

#include "glm/glm.hpp"

class Tile
{
public:
	Tile() = default;
	~Tile() = default;

	void Reset();

	// Setters
	void SetTextureIndex(size_t textureIndex) { m_TextureIndex = textureIndex; }

	// Getters
	int GetTextureIndex() const { return m_TextureIndex; }
	bool UseTexture() const { return m_TextureIndex != -1; }

	// Operator
	bool operator==(const Tile& other) const;
private:
	int m_TextureIndex = -1;
};