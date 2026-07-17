#pragma once

#include "Panel.h"

#include "Tiles.h"

#include <memory>

namespace Tiles::Editor
{
	// A live preview of what export produces: renders the project's visible layers,
	// framed to the export region (or the painted content's bounds when the region
	// is disabled), into an offscreen target each frame.
	class PanelRenderPreview : public Panel
	{
	public:
		PanelRenderPreview(EditorHost& host);
		~PanelRenderPreview() = default;

		void Render() override;
		void Update() override;

	private:
		// Centers and zooms `camera` so the world rect [worldMin, worldMax] fits a
		// square preview of `previewSize` pixels, with a small margin.
		void FrameCamera(Tiles::Camera2D& camera, const glm::vec2& worldMin, const glm::vec2& worldMax, float previewSize) const;

	private:
		std::shared_ptr<Tiles::RenderTarget> m_RenderTarget;
		float m_TileSize = 64.0f;
	};
}
