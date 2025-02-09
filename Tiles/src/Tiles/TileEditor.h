#include <vector>
#include <glm/glm.hpp>

#include "Lumina/Renderer/Texture.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "TileLayer.h"
#include "TileExporter.h"

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
    void RenderExport(); 
    void RenderConsol(); 
private:
    Lumina::TextureAtlas m_Atlas;
    EditorSpecification m_Spec; 
    EditorModes m_Modes; 

    // Tile Layer
    TileLayer m_TileLayer;
    TileExporter m_TileExporter;

    std::string m_SavePath;
    std::string m_LoadPath;
    std::string m_AtlasPath;
    std::string m_ExportPath;

    int m_AtlasWidth;
    int m_AtlasHeight; 

    // Flags/Attributes
    int m_SelectedTextureIndex;
};