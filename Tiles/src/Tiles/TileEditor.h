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
private:
    void RenderTile(int index, int y, int x);
    void LoadTextures();
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
};