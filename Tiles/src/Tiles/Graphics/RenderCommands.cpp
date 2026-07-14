#include "RenderCommands.h"

#include "Core/Assert.h"

#include <glad/gl.h>

namespace Tiles
{
	// --- Viewport and clearing ---------------------------------------------

	// Sets the GL viewport rectangle.
	void RenderCommands::SetViewport(int x, int y, int width, int height)
	{
		TILES_ASSERT(width > 0 && height > 0, "Viewport dimensions must be greater than zero!");
		glViewport(x, y, width, height);
	}

	// Sets the color the framebuffer is cleared to.
	void RenderCommands::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	// Clears the color, depth, and stencil buffers.
	void RenderCommands::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	// Clears only the color buffer.
	void RenderCommands::ClearColor()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Clears only the depth buffer.
	void RenderCommands::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// Clears only the stencil buffer.
	void RenderCommands::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	// --- Depth testing -----------------------------------------------------

	// Enables the depth test.
	void RenderCommands::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	// Disables the depth test.
	void RenderCommands::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	// Sets the depth comparison function.
	void RenderCommands::SetDepthFunc(DepthFunc func)
	{
		glDepthFunc(ToGLEnum(func));
	}

	// Enables or disables writes to the depth buffer.
	void RenderCommands::SetDepthMask(bool writeEnabled)
	{
		glDepthMask(writeEnabled ? GL_TRUE : GL_FALSE);
	}

	// --- Blending ----------------------------------------------------------

	// Enables blending.
	void RenderCommands::EnableBlending()
	{
		glEnable(GL_BLEND);
	}

	// Disables blending.
	void RenderCommands::DisableBlending()
	{
		glDisable(GL_BLEND);
	}

	// Sets the source and destination blend factors.
	void RenderCommands::SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor)
	{
		glBlendFunc(ToGLEnum(sfactor), ToGLEnum(dfactor));
	}

	// Sets the blend equation.
	void RenderCommands::SetBlendEquation(BlendEquation mode)
	{
		glBlendEquation(ToGLEnum(mode));
	}

	// --- Face culling ------------------------------------------------------

	// Enables face culling.
	void RenderCommands::EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	// Disables face culling.
	void RenderCommands::DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	// Sets which face is culled.
	void RenderCommands::SetCullFace(CullFace mode)
	{
		glCullFace(ToGLEnum(mode));
	}

	// Sets which winding is treated as front-facing.
	void RenderCommands::SetFrontFace(FrontFace mode)
	{
		glFrontFace(ToGLEnum(mode));
	}

	// --- Scissor testing ---------------------------------------------------

	// Enables the scissor test.
	void RenderCommands::EnableScissorTest()
	{
		glEnable(GL_SCISSOR_TEST);
	}

	// Disables the scissor test.
	void RenderCommands::DisableScissorTest()
	{
		glDisable(GL_SCISSOR_TEST);
	}

	// Sets the scissor rectangle.
	void RenderCommands::SetScissorBox(int x, int y, int width, int height)
	{
		TILES_ASSERT(width >= 0 && height >= 0, "Scissor box dimensions must be non-negative!");
		glScissor(x, y, width, height);
	}

	// --- Polygon and line/point settings -----------------------------------

	// Sets the fill/line/point polygon mode for both faces.
	void RenderCommands::SetPolygonMode(PolygonMode mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, ToGLEnum(mode));
	}

	// Sets the rasterized line width.
	void RenderCommands::SetLineWidth(float width)
	{
		TILES_ASSERT(width > 0.0f, "Line width must be greater than zero!");
		glLineWidth(width);
	}

	// Sets the rasterized point size.
	void RenderCommands::SetPointSize(float size)
	{
		TILES_ASSERT(size > 0.0f, "Point size must be greater than zero!");
		glPointSize(size);
	}

	// Lets the vertex shader set gl_PointSize.
	void RenderCommands::EnableProgramPointSize()
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	// Restores fixed point size.
	void RenderCommands::DisableProgramPointSize()
	{
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	// --- Basic drawing -----------------------------------------------------

	// Draws count vertices from vao as non-indexed primitives.
	void RenderCommands::DrawArrays(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t offset)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");
		TILES_ASSERT(count > 0, "Vertex count must be greater than zero!");

		vao->Bind();
		glDrawArrays(ToGLEnum(primitive), offset, count);
		vao->Unbind();
	}

	// Draws vao using its whole index buffer.
	void RenderCommands::DrawElements(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");

		vao->Bind();
		const auto ib = vao->GetIndexBuffer();
		TILES_ASSERT(ib, "IndexBuffer cannot be null for indexed drawing!");
		glDrawElements(ToGLEnum(primitive), ib->GetCount(), GL_UNSIGNED_INT, nullptr);
		vao->Unbind();
	}

	// Draws the first indexCount indices of vao.
	void RenderCommands::DrawElementsWithCount(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t indexCount)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");
		TILES_ASSERT(indexCount > 0, "Index count must be greater than zero!");

		vao->Bind();
		glDrawElements(ToGLEnum(primitive), indexCount, GL_UNSIGNED_INT, nullptr);
		vao->Unbind();
	}

	// --- Instanced drawing -------------------------------------------------

	// Draws instanceCount copies of a non-indexed primitive range.
	void RenderCommands::DrawArraysInstanced(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t instanceCount, uint32_t offset)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");
		TILES_ASSERT(count > 0, "Vertex count must be greater than zero!");
		TILES_ASSERT(instanceCount > 0, "Instance count must be greater than zero!");

		vao->Bind();
		glDrawArraysInstanced(ToGLEnum(primitive), offset, count, instanceCount);
		vao->Unbind();
	}

	// Draws instanceCount copies using vao's index buffer.
	void RenderCommands::DrawElementsInstanced(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, uint32_t instanceCount)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");
		TILES_ASSERT(instanceCount > 0, "Instance count must be greater than zero!");

		vao->Bind();
		const auto ib = vao->GetIndexBuffer();
		TILES_ASSERT(ib, "IndexBuffer cannot be null for indexed drawing!");
		glDrawElementsInstanced(ToGLEnum(primitive), ib->GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
		vao->Unbind();
	}

	// --- Multi-draw --------------------------------------------------------

	// Issues drawCount non-indexed draws in one call from parallel first/count arrays.
	void RenderCommands::MultiDrawArrays(const std::shared_ptr<VertexArray>& vao, PrimitiveType primitive, const int* first, const int* count, int drawCount)
	{
		TILES_ASSERT(vao, "VertexArray cannot be null!");
		TILES_ASSERT(first, "First array cannot be null!");
		TILES_ASSERT(count, "Count array cannot be null!");
		TILES_ASSERT(drawCount > 0, "Draw count must be greater than zero!");

		vao->Bind();
		glMultiDrawArrays(ToGLEnum(primitive), first, count, drawCount);
		vao->Unbind();
	}

	// --- Convenience wrappers by primitive type ----------------------------

	// Draws count vertices as points.
	void RenderCommands::DrawPoints(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		DrawArrays(vao, PrimitiveType::Points, count, offset);
	}

	// Draws count vertices as separate lines.
	void RenderCommands::DrawLines(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		DrawArrays(vao, PrimitiveType::Lines, count, offset);
	}

	// Draws count vertices as a line strip.
	void RenderCommands::DrawLineStrips(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		DrawArrays(vao, PrimitiveType::LineStrip, count, offset);
	}

	// Draws triangles: indexed when count is 0, otherwise count array vertices.
	void RenderCommands::DrawTriangles(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		if (count == 0)
			DrawElements(vao, PrimitiveType::Triangles);
		else
			DrawArrays(vao, PrimitiveType::Triangles, count, offset);
	}

	// Draws count vertices as a triangle strip.
	void RenderCommands::DrawTriangleStrip(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		DrawArrays(vao, PrimitiveType::TriangleStrip, count, offset);
	}

	// Draws count vertices as a triangle fan.
	void RenderCommands::DrawTriangleFan(const std::shared_ptr<VertexArray>& vao, uint32_t count, uint32_t offset)
	{
		DrawArrays(vao, PrimitiveType::TriangleFan, count, offset);
	}

	// --- Indexed convenience wrappers --------------------------------------

	// Draws vao's indices as points.
	void RenderCommands::DrawPointsIndexed(const std::shared_ptr<VertexArray>& vao)
	{
		DrawElements(vao, PrimitiveType::Points);
	}

	// Draws vao's indices as separate lines.
	void RenderCommands::DrawLinesIndexed(const std::shared_ptr<VertexArray>& vao)
	{
		DrawElements(vao, PrimitiveType::Lines);
	}

	// Draws vao's indices as triangles.
	void RenderCommands::DrawTrianglesIndexed(const std::shared_ptr<VertexArray>& vao)
	{
		DrawElements(vao, PrimitiveType::Triangles);
	}

	// --- Enum conversions --------------------------------------------------

	unsigned int RenderCommands::ToGLEnum(PolygonMode mode)
	{
		switch (mode)
		{
		case PolygonMode::Fill:  return GL_FILL;
		case PolygonMode::Line:  return GL_LINE;
		case PolygonMode::Point: return GL_POINT;
		default: TILES_ASSERT(false, "Unknown polygon mode!"); return GL_FILL;
		}
	}

	unsigned int RenderCommands::ToGLEnum(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero:                     return GL_ZERO;
		case BlendFactor::One:                      return GL_ONE;
		case BlendFactor::SrcColor:                 return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor:         return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor:                 return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor:         return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha:                 return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha:         return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha:                 return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha:         return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor:            return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor:    return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha:            return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha:    return GL_ONE_MINUS_CONSTANT_ALPHA;
		default: TILES_ASSERT(false, "Unknown blend factor!"); return GL_ZERO;
		}
	}

	unsigned int RenderCommands::ToGLEnum(BlendEquation equation)
	{
		switch (equation)
		{
		case BlendEquation::Add:             return GL_FUNC_ADD;
		case BlendEquation::Subtract:        return GL_FUNC_SUBTRACT;
		case BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendEquation::Min:             return GL_MIN;
		case BlendEquation::Max:             return GL_MAX;
		default: TILES_ASSERT(false, "Unknown blend equation!"); return GL_FUNC_ADD;
		}
	}

	unsigned int RenderCommands::ToGLEnum(DepthFunc func)
	{
		switch (func)
		{
		case DepthFunc::Never:        return GL_NEVER;
		case DepthFunc::Less:         return GL_LESS;
		case DepthFunc::Equal:        return GL_EQUAL;
		case DepthFunc::LessEqual:    return GL_LEQUAL;
		case DepthFunc::Greater:      return GL_GREATER;
		case DepthFunc::NotEqual:     return GL_NOTEQUAL;
		case DepthFunc::GreaterEqual: return GL_GEQUAL;
		case DepthFunc::Always:       return GL_ALWAYS;
		default: TILES_ASSERT(false, "Unknown depth function!"); return GL_LESS;
		}
	}

	unsigned int RenderCommands::ToGLEnum(CullFace face)
	{
		switch (face)
		{
		case CullFace::Front:        return GL_FRONT;
		case CullFace::Back:         return GL_BACK;
		case CullFace::FrontAndBack: return GL_FRONT_AND_BACK;
		default: TILES_ASSERT(false, "Unknown cull face!"); return GL_BACK;
		}
	}

	unsigned int RenderCommands::ToGLEnum(FrontFace face)
	{
		switch (face)
		{
		case FrontFace::Clockwise:        return GL_CW;
		case FrontFace::CounterClockwise: return GL_CCW;
		default: TILES_ASSERT(false, "Unknown front face!"); return GL_CCW;
		}
	}

	unsigned int RenderCommands::ToGLEnum(PrimitiveType primitive)
	{
		switch (primitive)
		{
		case PrimitiveType::Points:        return GL_POINTS;
		case PrimitiveType::Lines:         return GL_LINES;
		case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
		case PrimitiveType::LineLoop:      return GL_LINE_LOOP;
		case PrimitiveType::Triangles:     return GL_TRIANGLES;
		case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
		case PrimitiveType::TriangleFan:   return GL_TRIANGLE_FAN;
		default: TILES_ASSERT(false, "Unknown primitive type!"); return GL_TRIANGLES;
		}
	}
}
