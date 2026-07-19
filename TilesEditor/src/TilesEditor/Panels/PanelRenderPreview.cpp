#include "PanelRenderPreview.h"

#include "../UI/Widgets.h"
#include "../Rendering/TileSceneRenderer.h"

#include <algorithm>

namespace Tiles::Editor
{
	// Creates the offscreen target the preview renders into.
	PanelRenderPreview::PanelRenderPreview(EditorHost& host) : Panel(host)
	{
		m_RenderTarget = Tiles::RenderTarget::Create(512, 512);
	}

	// Renders the preview window: an offscreen render of the visible layers framed
	// to the export region, blitted into the panel.
	void PanelRenderPreview::Render()
	{
		if (!ImGui::Begin("Render Preview", OpenFlag()))
		{
			ImGui::End();
			return;
		}

		if (!Ctx().HasProject())
		{
			UI::TextMuted("No project loaded.");
			ImGui::End();
			return;
		}

		auto project = Ctx().GetProject();
		const ExportRegion& region = project->GetExportRegion();
		const LayerStack& layerStack = project->GetLayerStack();

		// The world rect to frame: the export region if enabled, else the painted
		// content's bounds. A tile at coord c fills the world cell [c, c + 1].
		glm::vec2 worldMin;
		glm::vec2 worldMax;
		if (region.Enabled)
		{
			worldMin = { region.Min.x * m_TileSize, region.Min.y * m_TileSize };
			worldMax = { (region.Min.x + region.Size.x) * m_TileSize, (region.Min.y + region.Size.y) * m_TileSize };
		}
		else if (auto bounds = layerStack.GetBounds())
		{
			worldMin = { bounds->x * m_TileSize, bounds->y * m_TileSize };
			worldMax = { (bounds->z + 1) * m_TileSize, (bounds->w + 1) * m_TileSize };
		}
		else
		{
			UI::TextMuted("Nothing to preview -- paint some tiles or enable an export region.");
			ImGui::End();
			return;
		}

		// A square preview sized to the panel.
		const ImVec2 avail = ImGui::GetContentRegionAvail();
		const float previewSize = std::max(std::min(avail.x, avail.y), 64.0f);
		const ImVec2 previewDim(previewSize, previewSize);

		Tiles::Camera2D camera;
		FrameCamera(camera, worldMin, worldMax, previewSize);

		Tiles::Renderer2D::SetRenderTarget(m_RenderTarget);
		Tiles::Renderer2D::SetResolution(static_cast<uint32_t>(previewDim.x), static_cast<uint32_t>(previewDim.y));
		Tiles::Renderer2D::BeginFrame(camera.ViewProjection({ previewDim.x, previewDim.y }));

		// A dark canvas behind the tiles, covering the framed rect.
		Tiles::Renderer2D::DrawSquare({
			.Position = { (worldMin.x + worldMax.x) * 0.5f, (worldMin.y + worldMax.y) * 0.5f, -0.9f },
			.Size = { worldMax.x - worldMin.x, worldMax.y - worldMin.y },
			.Tint = { 0.10f, 0.10f, 0.10f, 1.0f },
		});

		// Every visible layer, bottom to top.
		const auto& atlases = project->GetTextureAtlases();
		for (size_t i = 0; i < layerStack.GetLayerCount(); ++i)
		{
			const TileLayer& layer = layerStack.GetLayer(i);
			if (layer.GetVisibility())
				DrawTileLayer(layer, i, m_TileSize, atlases, 0.0f, Host());
		}

		Tiles::Renderer2D::EndFrame();
		Tiles::Renderer2D::SetRenderTarget(nullptr);

		ImGui::Image(static_cast<ImTextureID>(m_RenderTarget->GetTexture()), previewDim);

		ImGui::End();
	}

	void PanelRenderPreview::Update()
	{
	}

	// Centers on the rect and zooms so its larger extent fits with a small margin.
	void PanelRenderPreview::FrameCamera(Tiles::Camera2D& camera, const glm::vec2& worldMin, const glm::vec2& worldMax, float previewSize) const
	{
		const glm::vec2 center = (worldMin + worldMax) * 0.5f;
		const glm::vec2 extent = worldMax - worldMin;
		const float largestExtent = std::max({ extent.x, extent.y, 1.0f });

		camera.Center = center;
		camera.Zoom = (previewSize / largestExtent) * 0.9f;
	}
}
