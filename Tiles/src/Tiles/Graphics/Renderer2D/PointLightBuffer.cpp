#include "RendererState.h"

namespace Tiles
{
	void PointLightBuffer::Init()
	{
		Buffer = UniformBuffer::Create(sizeof(PointLight) * MaxPointLights, BufferUsage::Dynamic);
		Base = new PointLight[MaxPointLights];
		Ptr = Base;
	}

	void PointLightBuffer::Shutdown()
	{
		Buffer.reset();

		delete[] Base;
		Base = nullptr;
		Ptr = nullptr;
	}

	void PointLightBuffer::Append(RendererState& state)
	{
		if (Count >= MaxPointLights)
		{
			TILES_LOG_WARN("Maximum number of point lights ({}) exceeded", MaxPointLights);
			return;
		}

		Ptr->Position = Position;
		Ptr->Intensity = Intensity;
		Ptr->Color = Color;
		Ptr->Radius = Radius;
		Ptr->BlendingMode = (float)Blend;
		Ptr->BlendingAlpha = BlendAlpha;
		Ptr->FalloffType = (float)FalloffType;
		Ptr->Falloff = Falloff;
		Ptr++;

		Count++;
		state.Stats.PointLightCount++;
	}

	void PointLightBuffer::Reset()
	{
		Count = 0;
		Ptr = Base;
	}

	void PointLightBuffer::Upload(RendererState& state)
	{
		if (Count > 0)
		{
			uint32_t lightDataSize = Count * sizeof(PointLight);
			Buffer->SetData(Base, lightDataSize);
		}
	}

	void PointLightBuffer::Bind() const
	{
		Buffer->Bind(BindingLocationPointLight);
	}

	void PointLightBuffer::Unbind() const
	{
		Buffer->Unbind();
	}
}
