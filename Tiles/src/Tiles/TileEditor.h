#include <vector>
#include <stack>
#include <glm/glm.hpp>

#include "imgui.h"

#include "Lumina/Renderer/Texture.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "TileLayer.h"

struct EditorSpecification
{
    uint32_t Width = 40;
    uint32_t Height = 40;

    float TileSize = 40.0f;
    float Zoom = 1.0f; 
};

struct EditorModes
{
    bool Erase = false;
    bool Fill = false;
};

class TileEditor
{
public:
    void Init();
    void Shutdown(); 

    void Render();
private:
    void RenderHeader();
    void RenderTools();
    void RenderLayerSelction(); 
    void RenderTiles();
    void RenderAttributes();
    void RenderTextureSelection();
private:
    Lumina::TextureAtlas m_Atlas;
    EditorSpecification m_Spec; 
    EditorModes m_Modes; 

    // Tile Layer
    TileLayer m_TileLayer;
    uint32_t m_ActiveLayer;
    glm::vec3 m_TileHovered; 

    std::vector<bool> m_ActiveLayers;

    std::string m_SavePath;
    std::string m_LoadPath;
    std::string m_AtlasPath;

    int m_AtlasWidth;
    int m_AtlasHeight; 

    // Flags/Attributes
    int m_SelectedTextureIndex;
};