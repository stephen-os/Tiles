#pragma once

#include "Lumina/Renderer/Texture.h"

#include <vector>
#include <string>

#include <glm/glm.hpp>

namespace Tiles
{

    class Atlas
    {
    public:
        Atlas() = default;
	    ~Atlas() = default;

        void Create(const std::string& path);
        void UpdateTexCoords();

        void Bind() const;
        void Unbind() const;

        bool IsCreated() const { return m_IsCreated; }

        // Setters
        void SelectTexture(size_t index) { m_SelectedTexture = index; }
        void DeselectTexture() { m_SelectedTexture = -1; }

        // Getters
        int& GetWidth() { return m_GridWidth; }
        int& GetHeight() { return m_GridHeight; }

        std::string GetPath() const { return m_Path; }

        glm::vec4 GetTexCoords(int index) const;
        glm::vec2 GetOffset(int index) const;

        unsigned int GetTextureID() { return m_Texture.GetID(); }

        size_t GetSelectedTexture() const { return m_SelectedTexture; }
        bool IsTextureSelected() const { return m_SelectedTexture != -1; }
    private:
        Lumina::Texture m_Texture;              // Lumina Texture
        std::string m_Path = "";                // Atlas path
        int m_GridWidth = 16;                   // Number of textures along the width
        int m_GridHeight = 16;                  // Number of textures along the height
        float m_TexWidth = 0.0f;                // Width of a single texture in UV space
        float m_TexHeight = 0.0f;               // Height of a single texture in UV space
        bool m_IsCreated = false;               // Is the atlas created
        int m_SelectedTexture = -1;             // Selected Texture: -1 for none
        std::vector<glm::vec4> m_TexCoords;     // Precomputed texture coordinates
    };

}