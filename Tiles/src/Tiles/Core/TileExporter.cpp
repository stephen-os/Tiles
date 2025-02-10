#include "TileExporter.h"

#include "Lumina/Renderer/ShaderProgram.h"
#include "Lumina/Renderer/FrameBuffer.h"
#include "Lumina/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "TileObject.h"
void TileExporter::Export(TileLayer& layer, Lumina::TextureAtlas& atlas, std::string& filename)
{
    TileObject tileObject;

    const std::string vertexShader = R"(
        #version 330 core

        // Per Vertex
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec3 a_Normal;
        layout(location = 2) in vec2 a_TexCoord;

        uniform mat4 u_Transform;  // Per-tile transformation
        uniform mat4 u_OrthoProjection;  // Covers the entire tile grid
        uniform float u_NumberOfRows; 
        uniform vec2 u_Offset; 

        out vec3 v_Normal;
        out vec2 v_TexCoord;
        out vec3 v_Position;

        void main()
        {
            // Apply only the per-tile transform and the orthographic projection
            gl_Position = u_OrthoProjection * u_Transform * vec4(a_Position, 1.0);
            v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
            v_Position = vec3(u_Transform * vec4(a_Position, 1.0));
            v_TexCoord = (a_TexCoord / u_NumberOfRows) + u_Offset;
        }
    )";

    const std::string fragmetShader = R"(
        #version 330 core

        in vec3 v_Normal;
        in vec3 v_Position;
        in vec2 v_TexCoord;

        out vec4 FragColor;

        uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
        uniform sampler2D u_Texture;

        void main()
        {
            // Uniform lighting (simulate ambient lighting)
            vec3 uniformLighting = lightColor;

            // Texture color
            vec4 texColor = texture(u_Texture, v_TexCoord);
            if (texColor.a < 0.1)
                discard;

            FragColor = texColor;
        }
    )";

    Lumina::ShaderProgram shader;
    shader.SetSource(vertexShader, fragmetShader);

    int outputWidth = layer.LayerWidth() * m_Resolution;
    int outputHeight = layer.LayerHeight() * m_Resolution;

    glm::mat4 orthoProjection = glm::ortho(0.0f, float(layer.LayerWidth()), 0.0f, float(layer.LayerHeight()), -1.0f, 2.0f);

    Lumina::Renderer renderer;

    renderer.Init();
    renderer.Begin();
    renderer.OnWindowResize(outputWidth, outputHeight);
    renderer.Clear();

    atlas.Bind(); 

    shader.Bind();
    shader.SetUniformMatrix4fv("u_OrthoProjection", orthoProjection);
    shader.SetUniform1f("u_NumberOfRows", static_cast<float>(atlas.GetGridWidth()));

    for (int l = 0; l < layer.LayerSize(); l++)
    {
        for (int y = 0; y < layer.LayerHeight(); y++)
        {
            for (int x = 0; x < layer.LayerWidth(); x++)
            {
                TileData& tile = layer.GetTile(l, y, x);
                if (tile.UseTexture)
                {
                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, l * 0.01f));
                    glm::vec2 offset = atlas.GetOffset(static_cast<int>(tile.TextureIndex));

                    shader.SetUniformMatrix4fv("u_Transform", transform);
                    shader.SetUniform2fv("u_Offset", offset);
                    tileObject.Draw();
                }
            }
        }
    }

    renderer.End();
    renderer.SaveFrameBufferToImage(filename);
}