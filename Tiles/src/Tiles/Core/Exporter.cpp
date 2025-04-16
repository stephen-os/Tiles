#include "Exporter.h"

#include "Lumina/Core/Log.h"

#include "Lumina/Renderer/Cameras/OrthographicCamera.h"
#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/FrameBuffer.h"
#include "Lumina/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Tile.h"
#include "Layer.h"

namespace Tiles
{

    void Exporter::Export(Shared<Layers>& layers, Shared<Lumina::TextureAtlas>& atlas, std::string& filepath, std::string& filename, std::vector<size_t>& groupings)
    {
        return; 

        if (!atlas->HasTexture())
        {
            LUMINA_LOG_WARN("No atlas has been created.");
            return;
        }

        int outputWidth = static_cast<int>(layers->GetWidth() * m_Resolution);
        int outputHeight = static_cast<int>(layers->GetHeight() * m_Resolution);

        Lumina::OrthographicCamera camera(0.0f, layers->GetWidth(), 0.0f, layers->GetHeight(), -1.0f, 2.0f);

        Lumina::QuadAttributes attributes;
        attributes.Size = { 0.5f, 0.5f };
        attributes.Texture = atlas->GetTexture(); 

        size_t offset = 0;
        size_t index = 1; 
        for (size_t group : groupings)
		{
            if (group == 0)
                continue;

            Lumina::Renderer::Begin(camera);
            Lumina::Renderer::SetResolution(outputWidth, outputHeight);
            
            for (size_t l = offset; l < (offset + group) && l < layers->GetSize(); l++)
            {
                Layer& layer = layers->GetLayer(l);
                for (size_t y = 0; y < layer.GetHeight(); y++)
                {
                    for (size_t x = 0; x < layer.GetWidth(); x++)
                    {
                        Tile& tile = layer.GetTile(y, x);
                        if (tile.GetTextureIndex() != -1)
                        {
                            attributes.Position = { x + 0.5f, y + 0.5f, 0.0f };
                            attributes.TextureCoords = atlas->GetTextureCoords(tile.GetTextureIndex());

                            Lumina::Renderer::DrawQuad(attributes);
                        }
                    }
                }
            }

            Lumina::Renderer::End();
            Lumina::Renderer::SaveFrameBufferToFile(filepath + "/" + filename + "-" + std::to_string(index) + ".png");

            offset += group;
            index++;
		}
    }

}