#include <vector>
#include <stack>
#include <glm/glm.hpp>

#include "imgui.h"

#include "Lumina/Renderer/Texture.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "TileLayer.h"

class TileEditor
{
public:
    TileEditor();
    ~TileEditor() = default;

    void Init(int width, int height);
    void Shutdown(); 

    void Render();
    std::vector<glm::mat4>& GetMatrices() { return m_Matrices; }
    std::vector<glm::vec2>& GetOffsets() { return m_Offsets; }
private:
    void LoadTextures();

    void UpdateMatrices();
private:
    Lumina::TextureAtlas m_Atlas;

    // Tile Layer
    TileLayer m_TileLayer;
    int m_ActiveLayer;

    // Tile Settings
    float m_Padding;
    float m_TileSize;

    // File 
    std::string m_CurrentScenePath;
    std::string m_TileAtlasPath;

    // Flags/Attributes
    bool m_EraserMode;
    bool m_FillMode;
    float m_Opacity;
    int m_SelectedTextureIndex;

    // Render Data
    std::vector<glm::mat4> m_Matrices;
    std::vector<glm::vec2> m_Offsets;
};