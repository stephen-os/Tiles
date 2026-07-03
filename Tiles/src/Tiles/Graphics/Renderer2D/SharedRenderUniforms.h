#pragma once

#include "../ShaderProgram.h"

#include <memory>

namespace Tiles
{
	struct RendererState;

	// Applies the view-projection, wireframe, and lighting uniforms shared by the
	// quad, circle, line, text, and triangle shaders. Pixel and grid use reduced
	// uniform sets and apply theirs inline.
	void SetSharedUniforms(const std::shared_ptr<ShaderProgram>& shader, const RendererState& state);
}
