#include "SharedRenderUniforms.h"

#include "RendererState.h"

namespace Tiles
{
	void SetSharedUniforms(const std::shared_ptr<ShaderProgram>& shader, const RendererState& state)
	{
		shader->SetUniformMat4("u_ViewProjection", state.ViewProjectionMatrix);
		shader->SetUniformInt("u_WireframeMode", (int)state.PolygonMode);
		shader->SetUniformVec3("u_WireframeColor", state.WireFrameColor);
		shader->SetUniformInt("u_EnableLighting", (int)state.UseLighting);
		shader->SetUniformVec3("u_AmbientColor", state.AmbientColor);
		shader->SetUniformFloat("u_AmbientIntensity", state.AmbientIntensity);
		shader->SetUniformInt("u_PointLightCount", (int)state.PointLights.Count);
	}
}
