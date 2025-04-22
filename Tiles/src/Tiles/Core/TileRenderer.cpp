#include "TileRenderer.h"

#include "Lumina/Renderer/Renderer.h"

#include "Lumina/Core/Assert.h"
#include "Lumina/Core/Log.h"

#include <algorithm>

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

	void TileRenderer::Zoom(float delta)
	{
		s_Zoom = std::max(1.0f, std::min(20.0f, s_Zoom - delta));
	}

	float TileRenderer::GetZoom()
	{
		return s_Zoom;
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

	void TileRenderer::DrawGrid(const Layers& layers)
	{
		s_BackgroundAttributes.Shader->Bind();
		s_BackgroundAttributes.Shader->SetUniformVec2("u_GridSize", { 4.0f * layers.GetWidth(), 4.0f * layers.GetHeight() });
		s_BackgroundAttributes.Shader->Unbind();

		Lumina::Renderer::DrawQuad(s_BackgroundAttributes);
	}

	void TileRenderer::DrawLayers(const Layers& layers, const Lumina::TextureAtlas& atlas)
	{
		for (size_t layerIndex = 0; layerIndex < layers.GetSize(); layerIndex++)
		{
			const Layer& layer = layers.GetLayer(layerIndex);

			if (!layer.GetVisibility())
				continue;

			DrawLayer(layer, atlas);
		}
	}

	void TileRenderer::DrawLayer(const Layer& layer, const Lumina::TextureAtlas& atlas)
	{
		for (size_t row = 0; row < layer.GetHeight(); row++)
		{
			for (size_t col = 0; col < layer.GetWidth(); col++)
			{
				const Tile& tile = layer.GetTile(row, col);
				
				// Skip empty tiles
				if (tile.GetTextureIndex() == -1)
					continue;

				DrawTile(tile, atlas, row, col);
			}
		}
	}

	void TileRenderer::DrawTile(const Tile& tile, const Lumina::TextureAtlas& atlas, size_t row, size_t col)
	{
		if (atlas.HasTexture())
			s_TileAttributes.Texture = atlas.GetTexture();
		else
			s_TileAttributes.Texture = nullptr;

		s_TileAttributes.Position = { col * s_TileArea + s_TileOffset, row * s_TileArea + s_TileOffset, 0.0f };
		s_TileAttributes.TintColor = tile.GetTintColor();
		s_TileAttributes.TextureCoords = atlas.GetTextureCoords(tile.GetTextureIndex());
		s_TileAttributes.Rotation = tile.GetRotation();

		Lumina::Renderer::DrawQuad(s_TileAttributes);
	}

	void TileRenderer::ExportLayers(const Layers& layers, const Lumina::TextureAtlas& atlas, const ExportAttributes& exportAttributes)
	{
		LUMINA_LOG_INFO("Exporting layers: ");
		LUMINA_LOG_INFO("Filepath: {}", exportAttributes.Filepath);
		LUMINA_LOG_INFO("Filename: {}", exportAttributes.Filename);
		LUMINA_LOG_INFO("Resolution: {}", exportAttributes.Resolution);

		int outputWidth = static_cast<int>(layers.GetWidth() * exportAttributes.Resolution);
		int outputHeight = static_cast<int>(layers.GetHeight() * exportAttributes.Resolution);

		Lumina::OrthographicCamera camera(0.0f, layers.GetWidth(), 0.0f, layers.GetHeight(), -1.0f, 2.0f);

		Lumina::QuadAttributes tileAttributes;
		tileAttributes.Size = { 0.5f, 0.5f };

		if (atlas.HasTexture())
			tileAttributes.Texture = atlas.GetTexture();
		else
			tileAttributes.Texture = nullptr; 

		size_t decorator = 1;
		size_t layersRendered = 0; 

		LUMINA_LOG_INFO("Groupings: ");
		for (size_t group = 0; group < layers.GetSize(); group++)
		{
			Lumina::Renderer::Begin(camera);
			Lumina::Renderer::SetResolution(outputWidth, outputHeight);

			for (size_t layerIndex = 0; layerIndex < layers.GetSize(); layerIndex++)
			{
				const Layer& layer = layers.GetLayer(layerIndex);
				if (layer.GetRenderGroup() != group)
					continue;

				LUMINA_LOG_INFO("- Rendering Layer {} On Group {}", layerIndex, group); 
				for (size_t y = 0; y < layer.GetHeight(); y++)
				{
					for (size_t x = 0; x < layer.GetWidth(); x++)
					{
						const Tile& tile = layer.GetTile(y, x);
						if (tile.GetTextureIndex() != -1)
						{
							tileAttributes.Position = { x + 0.5f, y + 0.5f, 0.0f };
							tileAttributes.TextureCoords = atlas.GetTextureCoords(tile.GetTextureIndex());

							Lumina::Renderer::DrawQuad(tileAttributes);
						}
					}
				}
				layersRendered++;
			}

			Lumina::Renderer::End();

			if (layersRendered != 0)
			{
				Lumina::Renderer::SaveFrameBufferToFile(exportAttributes.Filepath + "/" + exportAttributes.Filename + "-" + std::to_string(decorator) + ".png");
				layersRendered = 0; 
				decorator++;
			}
			else
			{
				LUMINA_LOG_INFO("- 0 Layers On Group {}", group);
			}
		}
		LUMINA_LOG_INFO("Export completed.");
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