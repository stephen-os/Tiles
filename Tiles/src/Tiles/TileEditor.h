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
};

struct EditorModes
{
    bool Erase = false;
    bool Fill = false;
};

class TileEditor
{
public:
    TileEditor();
    ~TileEditor() = default;

    void Init(int width, int height);
    void Shutdown(); 

    void Render();
private:
    void RenderTiles();
private:
    Lumina::TextureAtlas m_Atlas;
    EditorSpecification m_Spec; 
    EditorModes m_Modes; 

    // Tile Layer
    TileLayer m_TileLayer;
    int m_ActiveLayer;

    std::vector<bool> m_ActiveLayers;

    // Flags/Attributes
    int m_SelectedTextureIndex;
};