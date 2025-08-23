#pragma once
#include "imgui.h"
#include "../Core/Context.h"
#include "../Core/Base.h"
#include <string>
#include <vector>
#include <map>

namespace Tiles
{
    enum class ExportFormat
    {
        PNG = 0,
        JPG,
        BMP,
        TGA
    };

    class PopupRenderMatrix
    {
    public:
        PopupRenderMatrix();
        ~PopupRenderMatrix() = default;
        void Show(Ref<Context> context);
        void Render();
        bool IsVisible() const { return m_IsVisible; }
        void Close() { m_IsVisible = false; }
    private:
        void RenderLayerMatrix();
        void RenderExportSettings();
        void RenderActionButtons();
        void ShowDirectoryDialog();
        void ResetToDefaults();
        void ExecuteExport();
        std::vector<std::string> GetUsedRenderGroups() const;
        std::string GetExportFileName(int groupIndex = -1) const;
        std::string GetFullExportPath(const std::string& fileName) const;
        const char* GetFormatExtension() const;
        void ExportRenderGroup(const std::vector<size_t>& layerIndices, const std::string& fileName);
    private:
        bool m_IsVisible = false;
        Ref<Context> m_Context;

        char m_ExportFileName[256] = "export";
        std::string m_ExportDirectory = ".";
        ExportFormat m_ExportFormat = ExportFormat::PNG;

        std::map<size_t, int> m_LayerToRenderGroup;

        bool m_ExportVisible = true;
        bool m_ExportInvisible = false;
        bool m_UseProjectBounds = true;
        bool m_ShowDirectorySelector = false;

        static constexpr int RENDER_GROUPS = 8;
    };
}