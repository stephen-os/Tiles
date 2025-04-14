#include "TileRenderer.h"

namespace Tiles
{
	Lumina::QuadAttributes TileRenderer::s_BackgroundAttributes;
	Lumina::QuadAttributes TileRenderer::s_TileAttributes;

	void TileRenderer::Init()
	{
		std::string vertexShaderSource = "res/shaders/Background.vert";
		std::string fragmentShaderSource = "res/shaders/Background.frag";
		Shared<Lumina::ShaderProgram> shader = Lumina::ShaderProgram::Create(vertexShaderSource, fragmentShaderSource);

		s_BackgroundAttributes.Shader = shader;
		s_BackgroundAttributes.Size = { 5.0f, 5.0f };

		s_TileAttributes.Size = { 0.02f, 0.02f };
	}

	void TileRenderer::Render(Lumina::Camera& camera, Shared<Layers>& layers, Shared<Lumina::TextureAtlas> atlas, glm::vec2 & viewportSize, float& zoom)
	{
		// No layers, skip completely
		if (!layers)
			return;

		camera.SetProjectionMatrix(1.0f * zoom, viewportSize.x / viewportSize.y, 0.01f, 100.0f);

		Lumina::Renderer::Begin(camera);
		Lumina::Renderer::SetResolution(viewportSize.x, viewportSize.y);

		s_BackgroundAttributes.Shader->Bind();
		// * 4.0f because 4 small squares make 1 big square
		s_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * layers->GetWidth(), 4.0f * layers->GetHeight() });
		s_BackgroundAttributes.Shader->Unbind();

		// Draw Background
		Lumina::Renderer::DrawQuad(s_BackgroundAttributes);

		// No atlas, just render the background 
		if (!atlas->HasTexture())
		{
			Lumina::Renderer::End();
			return;
		}

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

					s_TileAttributes.Position = { x * 0.04f + 0.02f, y * 0.04f + 0.02f, 0.0f };
					s_TileAttributes.TextureCoords = atlas->GetTextureCoords(tile.GetTextureIndex());
					Lumina::Renderer::DrawQuad(s_TileAttributes);
				}
			}
		}

		Lumina::Renderer::End();
	}

	void* TileRenderer::GetImage()
	{
		return (void*)(intptr_t)Lumina::Renderer::GetImage();
	}
}