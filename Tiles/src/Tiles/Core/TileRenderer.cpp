#include "TileRenderer.h"

#include "Lumina/Renderer/Renderer.h"

#include "Lumina/Core/Assert.h"

namespace Tiles
{
	static Lumina::QuadAttributes s_BackgroundAttributes;
	static Lumina::QuadAttributes s_TileAttributes;

	static Lumina::OrthographicCamera s_Camera;

	static glm::vec2 s_Resolution = { 0.0f, 0.0f };
	static float s_Zoom = 10.0f;

	static float s_TileSize = 0.02f;					// Size of the tile relative to background quad. 
	static float s_TileOffset = 0.02f;					// Offset from origin or center of bacground quad.
	static float s_TileArea = 0.04f;					// Area of the tile relative to background quad.
	
	void TileRenderer::Init()
	{
		Lumina::Renderer::Init();

		std::string vertexShaderSource = "res/shaders/Background.vert";
		std::string fragmentShaderSource = "res/shaders/Background.frag";
		Shared<Lumina::ShaderProgram> shader = Lumina::ShaderProgram::Create(vertexShaderSource, fragmentShaderSource);

		s_Camera.SetPosition({ 0.9f, 0.9f, -1.0f });

		s_BackgroundAttributes.Shader = shader;
		s_BackgroundAttributes.Size = { 5.0f, 5.0f };

		s_TileAttributes.Size = { s_TileSize, s_TileSize };
	}

	void TileRenderer::Shutdown()
	{
		Lumina::Renderer::Shutdown();
	}

	void TileRenderer::SetResolution(glm::vec2 resolution)
	{
		s_Resolution = resolution;
	}

	void TileRenderer::SetZoom(float zoom)
	{
		s_Zoom = zoom;
	}

	void TileRenderer::Begin()
	{
		Lumina::Renderer::Begin(s_Camera);
		Lumina::Renderer::SetResolution(s_Resolution.x, s_Resolution.y);
		s_Camera.SetProjectionMatrix(s_Zoom, s_Resolution.x / s_Resolution.y, 0.01f, 100.0f);
	}

	void TileRenderer::End()
	{
		Lumina::Renderer::End();
	}

	void TileRenderer::DrawGrid(Shared<Layers>& layers)
	{
		s_BackgroundAttributes.Shader->Bind();
		s_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * layers->GetWidth(), 4.0f * layers->GetHeight() });
		s_BackgroundAttributes.Shader->Unbind();

		Lumina::Renderer::DrawQuad(s_BackgroundAttributes);
	}

	void TileRenderer::DrawLayers(Shared<Layers>& layers, Shared<Lumina::TextureAtlas> atlas)
	{
		LUMINA_ASSERT(layers, "Layers is null!");
		LUMINA_ASSERT(atlas, "Atlas is null!");
 
		// No texture, skip 
		if (!atlas->HasTexture())
			return;

		s_TileAttributes.Texture = atlas->GetTexture();

		// Draw Tiles
		for (size_t layerIndex = 0; layerIndex < layers->GetSize(); layerIndex++)
		{
			Layer& layer = layers->GetLayer(layerIndex);

			if (!layer.GetVisibility())
				continue;

			for (size_t y = 0; y < layer.GetHeight(); y++)
			{
				for (size_t x = 0; x < layer.GetWidth(); x++)
				{
					Tile& tile = layer.GetTile(y, x);

					// Skip empty tiles
					if (tile.GetTextureIndex() == -1)
						continue;

					s_TileAttributes.Position = { x * s_TileArea + s_TileOffset, y * s_TileArea + s_TileOffset, 0.0f };
					s_TileAttributes.TextureCoords = atlas->GetTextureCoords(tile.GetTextureIndex());
					Lumina::Renderer::DrawQuad(s_TileAttributes);
				}
			}
		}
	}

	void TileRenderer::DrawLayer(Layer& layer, Shared<Lumina::TextureAtlas> atlas)
	{
		LUMINA_ASSERT(atlas, "Atlas is null!");

		if (!atlas->HasTexture())
			return;

		s_TileAttributes.Texture = atlas->GetTexture();
		for (size_t y = 0; y < layer.GetHeight(); y++)
		{
			for (size_t x = 0; x < layer.GetWidth(); x++)
			{
				Tile& tile = layer.GetTile(y, x);
				// Skip empty tiles
				if (tile.GetTextureIndex() == -1)
					continue;

				s_TileAttributes.Position = { x * s_TileArea + s_TileOffset, y * s_TileArea + s_TileOffset, 0.0f };
				s_TileAttributes.TextureCoords = atlas->GetTextureCoords(tile.GetTextureIndex());

				Lumina::Renderer::DrawQuad(s_TileAttributes);
			}
		}
	}

	void TileRenderer::DrawTile(Tile& tile, Shared<Lumina::TextureAtlas> atlas, glm::vec2 position)
	{
		LUMINA_ASSERT(atlas, "Atlas is null!");

		if (!atlas->HasTexture())
			return;

		s_TileAttributes.Texture = atlas->GetTexture();
		s_TileAttributes.Position = { position.x * s_TileArea + s_TileOffset, position.y * s_TileArea + s_TileOffset, 0.0f };
		s_TileAttributes.TextureCoords = atlas->GetTextureCoords(tile.GetTextureIndex());

		Lumina::Renderer::DrawQuad(s_TileAttributes);
	}

	void* TileRenderer::GetImage()
	{
		return (void*)(intptr_t)Lumina::Renderer::GetImage();
	}

	Lumina::Camera& TileRenderer::GetCamera()
	{
		return s_Camera;
	}
}