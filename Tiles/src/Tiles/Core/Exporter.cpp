#include "Exporter.h"

#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/FrameBuffer.h"
#include "Lumina/Renderer/Renderer.h"
#include "Lumina/Renderer/Shapes/Quad.h"

#include "Lumina/Utils/FileReader.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Tile.h"
#include "Layer.h"

namespace Tiles
{

    void Exporter::Export(Shared<Layers>& layers, Shared<Atlas>& atlas, std::string& filepath, std::string& filename, std::vector<size_t>& groupings)
    {
        Lumina::Quad quad;

        const std::string vertexShader = Lumina::ReadFile("res/shaders/ExportShader.vert");
        const std::string fragmetShader = Lumina::ReadFile("res/shaders/ExportShader.frag");
       
        Lumina::ShaderProgram shader;
        shader.SetSource(vertexShader, fragmetShader);

        int outputWidth = layers->GetWidth() * m_Resolution;
        int outputHeight = layers->GetHeight() * m_Resolution;

        glm::mat4 orthoProjection = glm::ortho(0.0f, float(layers->GetWidth()), 0.0f, float(layers->GetHeight()), -1.0f, 2.0f);

        size_t offset = 0;
        size_t index = 1; 
        for (size_t group : groupings)
		{
            if (group == 0)
                continue;

            Lumina::Renderer::Begin();
            Lumina::Renderer::OnWindowResize(outputWidth, outputHeight);
            Lumina::Renderer::Clear();

            atlas->Bind();

            shader.Bind();
            shader.SetUniformMatrix4fv("u_OrthoProjection", orthoProjection);
            shader.SetUniform1f("u_NumberOfRows", static_cast<float>(atlas->GetWidth()));

            for (int l = offset; l < (offset + group) && l < layers->GetSize(); l++)
            {
                Layer& layer = layers->GetLayer(l);
                for (int y = 0; y < layer.GetHeight(); y++)
                {
                    for (int x = 0; x < layer.GetWidth(); x++)
                    {
                        Tile& tile = layer.GetTile(y, x);
                        if (tile.GetTextureIndex() != -1)
                        {
                            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, l * 0.01f));
                            glm::vec2 offset = atlas->GetOffset(static_cast<int>(tile.GetTextureIndex()));

                            shader.SetUniformMatrix4fv("u_Transform", transform);
                            shader.SetUniform2fv("u_Offset", offset);
                            Lumina::Renderer::Draw(quad.GetVertexArray());
                        }
                    }
                }
            }

            Lumina::Renderer::End();
            Lumina::Renderer::SaveFrameBufferToImage(filepath + "/" + filename + "-" + std::to_string(index) + ".png");

            offset += group;
            index++;
		}
    }

}