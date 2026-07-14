#pragma once

#include "VertexArray.h"

#include <cstdint>
#include <memory>

namespace Tiles
{
	enum class PolygonMode
	{
		Fill = 0,
		Line,
		Point
	};

	enum class BlendFactor
	{
		Zero = 0,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha
	};

	enum class BlendEquation
	{
		Add = 0,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class DepthFunc
	{
		Never = 0,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	enum class CullFace
	{
		Front = 0,
		Back,
		FrontAndBack
	};

	enum class FrontFace
	{
		Clockwise = 0,
		CounterClockwise
	};

	enum class PrimitiveType
	{
		Points = 0,
		Lines,
		LineStrip,
		LineLoop,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	// A thin, stateless facade over the OpenGL pipeline-state and draw calls,
	// taking engine enums instead of raw GL constants. All methods are static;
	// GL errors surface through the debug-message callback, not a return value.
	class RenderCommands
	{
	public:
		// Viewport and clearing
		static void SetViewport(int x, int y, int width, int height);
		static void SetClearColor(float r, float g, float b, float a = 1.0f);
		static void Clear();
		static void ClearColor();
		static void ClearDepth();
		static void ClearStencil();

		// Depth testing
		static void EnableDepthTest();
		static void DisableDepthTest();
		static void SetDepthFunc(DepthFunc func);
		static void SetDepthMask(bool writeEnabled);

		// Blending
		static void EnableBlending();
		static void DisableBlending();
		static void SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor);
		static void SetBlendEquation(BlendEquation mode);

		// Face culling
		static void EnableFaceCulling();
		static void DisableFaceCulling();
		static void SetCullFace(CullFace mode);
		static void SetFrontFace(FrontFace mode);

		// Scissor testing
		static void EnableScissorTest();
		static void DisableScissorTest();
		static void SetScissorBox(int x, int y, int width, int height);

		// Polygon and line/point settings
		static void SetPolygonMode(PolygonMode mode);
		static void SetLineWidth(float width);
		static void SetPointSize(float size);
		static void EnableProgramPointSize();
		static void DisableProgramPointSize();

		// Basic drawing
		static void DrawArrays(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t offset = 0);
		static void DrawElements(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive);
		static void DrawElementsWithCount(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t indexCount);

		// Instanced drawing
		static void DrawArraysInstanced(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t instanceCount, uint32_t offset = 0);
		static void DrawElementsInstanced(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t instanceCount);

		// Multi-draw
		static void MultiDrawArrays(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, const int* first, const int* count, int drawCount);

		// Convenience wrappers by primitive type
		static void DrawPoints(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
		static void DrawLines(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
		static void DrawLineStrips(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
		static void DrawTriangles(const std::shared_ptr<VertexArray>& vao, uint32_t count = 0, uint32_t offset = 0); // count = 0 uses the index buffer
		static void DrawTriangleStrip(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
		static void DrawTriangleFan(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset = 0);

		// Indexed convenience wrappers
		static void DrawPointsIndexed(const std::shared_ptr<VertexArray>& vao);
		static void DrawLinesIndexed(const std::shared_ptr<VertexArray>& vao);
		static void DrawTrianglesIndexed(const std::shared_ptr<VertexArray>& vao);

	private:
		// Engine enum -> GL constant conversions.
		static unsigned int ToGLEnum(PolygonMode mode);
		static unsigned int ToGLEnum(BlendFactor factor);
		static unsigned int ToGLEnum(BlendEquation equation);
		static unsigned int ToGLEnum(DepthFunc func);
		static unsigned int ToGLEnum(CullFace face);
		static unsigned int ToGLEnum(FrontFace face);
		static unsigned int ToGLEnum(PrimitiveType primitive);
	};
}
