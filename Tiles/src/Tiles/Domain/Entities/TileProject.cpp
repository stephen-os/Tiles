#include "TileProject.h"

#include <algorithm>
#include <stdexcept>

namespace Tiles::Domain
{
    TileProject::TileProject(uint32_t width, uint32_t height, const std::string& name)
        : m_Name(name)
        , m_Width(width)
        , m_Height(height)
        , m_CreatedTime(std::chrono::steady_clock::now())
        , m_LastModifiedTime(m_CreatedTime)
    {
        // Create default layer
        m_Layers.push_back(std::make_unique<TileGrid>(width, height, "Layer 1"));
    }

    void TileProject::SetName(const std::string& name)
    {
        if (m_Name != name)
        {
            m_Name = name;
            MarkAsModified();
        }
    }

    void TileProject::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width == width && m_Height == height)
            return;

        m_Width = width;
        m_Height = height;

        for (auto& layer : m_Layers)
        {
            layer->Resize(width, height);
        }

        MarkAsModified();
    }

    void TileProject::MarkAsModified()
    {
        m_IsDirty = true;
        m_LastModifiedTime = std::chrono::steady_clock::now();
    }

    void TileProject::MarkAsSaved()
    {
        m_IsDirty = false;
        m_LastSavedTime = std::chrono::steady_clock::now();
    }

    TileGrid& TileProject::GetLayer(size_t index)
    {
        if (index >= m_Layers.size())
        {
            throw std::out_of_range("Layer index out of range");
        }
        return *m_Layers[index];
    }

    const TileGrid& TileProject::GetLayer(size_t index) const
    {
        if (index >= m_Layers.size())
        {
            throw std::out_of_range("Layer index out of range");
        }
        return *m_Layers[index];
    }

    size_t TileProject::AddLayer(const std::string& name)
    {
        m_Layers.push_back(std::make_unique<TileGrid>(m_Width, m_Height, name));
        MarkAsModified();
        return m_Layers.size() - 1;
    }

    void TileProject::RemoveLayer(size_t index)
    {
        if (index >= m_Layers.size())
            return;

        // Always keep at least one layer
        if (m_Layers.size() <= 1)
            return;

        m_Layers.erase(m_Layers.begin() + static_cast<ptrdiff_t>(index));
        MarkAsModified();
    }

    void TileProject::MoveLayerUp(size_t index)
    {
        if (index == 0 || index >= m_Layers.size())
            return;

        std::swap(m_Layers[index], m_Layers[index - 1]);
        MarkAsModified();
    }

    void TileProject::MoveLayerDown(size_t index)
    {
        if (index >= m_Layers.size() - 1)
            return;

        std::swap(m_Layers[index], m_Layers[index + 1]);
        MarkAsModified();
    }

    void TileProject::SwapLayers(size_t index1, size_t index2)
    {
        if (index1 >= m_Layers.size() || index2 >= m_Layers.size())
            return;

        std::swap(m_Layers[index1], m_Layers[index2]);
        MarkAsModified();
    }

    void TileProject::InsertLayer(size_t index, const std::string& name)
    {
        if (index > m_Layers.size())
            index = m_Layers.size();

        m_Layers.insert(
            m_Layers.begin() + static_cast<ptrdiff_t>(index),
            std::make_unique<TileGrid>(m_Width, m_Height, name)
        );
        MarkAsModified();
    }

    void TileProject::DuplicateLayer(size_t index)
    {
        if (index >= m_Layers.size())
            return;

        const auto& sourceLayer = *m_Layers[index];
        auto newLayer = std::make_unique<TileGrid>(m_Width, m_Height, sourceLayer.GetName() + " (Copy)");
        newLayer->SetVisible(sourceLayer.IsVisible());
        newLayer->SetRawData(sourceLayer.GetRawData());

        m_Layers.insert(
            m_Layers.begin() + static_cast<ptrdiff_t>(index) + 1,
            std::move(newLayer)
        );
        MarkAsModified();
    }

    void TileProject::AddAtlasReference(const std::string& atlasPath)
    {
        m_AtlasReferences.push_back(atlasPath);
        MarkAsModified();
    }

    void TileProject::RemoveAtlasReference(size_t index)
    {
        if (index >= m_AtlasReferences.size())
            return;

        m_AtlasReferences.erase(m_AtlasReferences.begin() + static_cast<ptrdiff_t>(index));
        MarkAsModified();
    }

    void TileProject::ClearAtlasReferences()
    {
        m_AtlasReferences.clear();
        MarkAsModified();
    }
}
