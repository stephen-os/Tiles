#include <vector>
#include <glm/glm.hpp>

#include "Lumina/Renderer/Texture.h"
#include "Lumina/Renderer/TextureAtlas.h"

#include "TileLayer.h"
#include "TileExporter.h"

// Panels
#include "panels/TextureSelectionPanel.h"
#include "panels/TileViewportPanel.h"

class TileEditor
{
public:
    void Init();
    void Shutdown(); 

    void Render();
private:
    void RenderTools();
    void RenderLayerSelction(); 
    void RenderAttributes();
    void RenderExport();
private:
    // Tile Layer
    TileLayer m_TileLayer;
    TileExporter m_TileExporter;

    std::string m_ExportPath;

    // Panels
    TextureSelectionPanel m_TextureSelectionPanel;
    TileViewportPanel m_TileViewportPanel;
};