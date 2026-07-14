#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace Tiles
{
	// Linked vertex+fragment GL program; RAII wrapper owning the program and
	// its two shader objects. Active uniform locations are cached at link time.
	class ShaderProgram
	{
	public:
		// Compiles, links, and caches uniforms for a vertex+fragment program.
		[[nodiscard]] static std::shared_ptr<ShaderProgram> Create(const std::string& vertexSource, const std::string& fragmentSource);

		// Compiles both stages from GLSL source, links them, and caches the
		// locations of all active uniforms.
		ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
		~ShaderProgram();

		// Makes this program the active GL program.
		void Bind() const;

		// Unbinds any program.
		void Unbind() const;

		// The GL location of a vertex attribute by name, or -1 (with a log) if absent.
		[[nodiscard]] int GetAttributeLocation(const std::string& name) const;

		// Sets a scalar/vector/matrix uniform by name; a no-op for an unknown name.
		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformVec2(const std::string& name, float a, float b);
		void SetUniformVec2(const std::string& name, const glm::vec2& value);
		void SetUniformVec3(const std::string& name, float a, float b, float c);
		void SetUniformVec3(const std::string& name, const glm::vec3& value);
		void SetUniformVec4(const std::string& name, float a, float b, float c, float d);
		void SetUniformVec4(const std::string& name, const glm::vec4& value);
		void SetUniformMat4(const std::string& name, const glm::mat4& value);

		// The GL program handle.
		[[nodiscard]] uint32_t GetID() const { return m_ShaderProgramID; }

	private:
		// Compiles one shader stage (@p type is GL_VERTEX_SHADER/GL_FRAGMENT_SHADER)
		// from source, logging and returning 0 on failure.
		[[nodiscard]] uint32_t CompileSource(uint32_t type, const std::string& source);

		// The cached location of @p name, or -1 (with a log) if it is not an active uniform.
		[[nodiscard]] int UniformLocation(const std::string& name) const;

		uint32_t m_VertexShaderID = 0;
		uint32_t m_FragmentShaderID = 0;
		uint32_t m_ShaderProgramID = 0;

		std::unordered_map<std::string, int> m_Uniforms;
	};
}
