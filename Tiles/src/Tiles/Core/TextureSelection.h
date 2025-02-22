// Selection.h
#pragma once

#include <set>
#include <vector>

namespace Tiles
{

    class TextureSelection
    {
    public:
        TextureSelection() = default;
        ~TextureSelection() = default;

        void Select(int texture);
        void Deselect(int texture) { m_SelectedTextures.erase(texture); }
        void Clear() { m_SelectedTextures.clear(); }

        int Front() const { return *m_SelectedTextures.begin(); }
        size_t Size() const { return m_SelectedTextures.size(); }
        bool Empty() const { return m_SelectedTextures.empty(); }

        auto begin() const { return m_SelectedTextures.begin(); }
        auto end() const { return m_SelectedTextures.end(); }

        bool IsSelected(int texture) const { return m_SelectedTextures.find(texture) != m_SelectedTextures.end(); }
        std::vector<int> GetSelectedTextures() const { return std::vector<int>(m_SelectedTextures.begin(), m_SelectedTextures.end()); }
    private:
        std::set<int> m_SelectedTextures;
        bool m_MultiSelect = false;
    };
}