#include "ShaderProgram.h"

#include "Core/Assert.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace Tiles
{
	// Compiles, links, and caches uniforms for a vertex+fragment program.
	std::shared_ptr<ShaderProgram> ShaderProgram::Create(const std::string& vertexSource, const std::string& fragmentSource)
	{
		return std::make_shared<ShaderProgram>(vertexSource, fragmentSource);
	}

	// Compiles both stages, links them, and caches active uniform locations.
	ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
	{
		m_VertexShaderID = CompileSource(GL_VERTEX_SHADER, vertexSource);
		TILES_ASSERT(m_VertexShaderID != 0, "Vertex shader compilation failed!");

		m_FragmentShaderID = CompileSource(GL_FRAGMENT_SHADER, fragmentSource);
		TILES_ASSERT(m_FragmentShaderID != 0, "Fragment shader compilation failed!");

		// Link the two compiled stages into a program.
		m_ShaderProgramID = glCreateProgram();
		TILES_ASSERT(m_ShaderProgramID != 0, "Failed to create shader program!");

		glAttachShader(m_ShaderProgramID, m_VertexShaderID);
		glAttachShader(m_ShaderProgramID, m_FragmentShaderID);
		glLinkProgram(m_ShaderProgramID);

		int isOkay;
		glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &isOkay);
		if (!isOkay)
		{
			char message[512];
			glGetProgramInfoLog(m_ShaderProgramID, 512, nullptr, message);
			glDeleteProgram(m_ShaderProgramID);
			m_ShaderProgramID = 0;

			TILES_ENGINE_ERROR("Shader program linking error:\n{0}", message);
			return;
		}

		// Cache the location of every active uniform so SetUniform* can look up
		// by name without a GL round-trip per call.
		m_Uniforms.clear();
		int nuniforms;
		glGetProgramiv(m_ShaderProgramID, GL_ACTIVE_UNIFORMS, &nuniforms);
		for (GLint i = 0; i < nuniforms; ++i)
		{
			char name[256];
			int length;
			int size;
			unsigned int type;
			glGetActiveUniform(m_ShaderProgramID, i, 256, &length, &size, &type, name);

			int location = glGetUniformLocation(m_ShaderProgramID, name);
			m_Uniforms[name] = location;

			// Array uniforms report only their [0] element; strip that suffix and
			// cache each remaining element ("name[1]", "name[2]", ...) explicitly.
			for (int elementIndex = 1; elementIndex < size; ++elementIndex)
			{
				std::string elementName = std::string(name).substr(0, length - 3) + "[" + std::to_string(elementIndex) + "]";
				int elementLocation = glGetUniformLocation(m_ShaderProgramID, elementName.c_str());
				if (elementLocation != -1)
					m_Uniforms[elementName] = elementLocation;
			}
		}

		Unbind();
	}

	// Deletes the two shader objects and the program.
	ShaderProgram::~ShaderProgram()
	{
		glDeleteShader(m_VertexShaderID);
		glDeleteShader(m_FragmentShaderID);
		glDeleteProgram(m_ShaderProgramID);
	}

	// Compiles one shader stage from source; logs and returns 0 on failure.
	uint32_t ShaderProgram::CompileSource(uint32_t type, const std::string& source)
	{
		uint32_t shader = glCreateShader(type);
		TILES_ASSERT(shader != 0, "Failed to create shader!");

		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char message[512];
			glGetShaderInfoLog(shader, 512, nullptr, message);
			glDeleteShader(shader);

			TILES_ENGINE_ERROR("Shader compilation error: {}\n{}", (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message);
			return 0;
		}

		return shader;
	}

	// Makes this program the active GL program.
	void ShaderProgram::Bind() const
	{
		TILES_ASSERT(m_ShaderProgramID != 0, "Cannot bind an invalid shader program!");
		glUseProgram(m_ShaderProgramID);
	}

	// Unbinds any program.
	void ShaderProgram::Unbind() const
	{
		glUseProgram(0);
	}

	// The GL location of a vertex attribute by name, or -1 (with a log) if absent.
	int ShaderProgram::GetAttributeLocation(const std::string& name) const
	{
		int location = glGetAttribLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			TILES_ENGINE_ERROR("Attribute location for {0} is not found", name);

		return location;
	}

	// The cached location of name, or -1 (with a log) if it is not an active uniform.
	int ShaderProgram::UniformLocation(const std::string& name) const
	{
		auto it = m_Uniforms.find(name);
		if (it == m_Uniforms.end())
		{
			TILES_ENGINE_ERROR("{0} isn't a valid uniform.", name);
			return -1;
		}
		return it->second;
	}

	// Sets an int uniform by name.
	void ShaderProgram::SetUniformInt(const std::string& name, int value)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform1i(location, value);
	}

	// Sets a float uniform by name.
	void ShaderProgram::SetUniformFloat(const std::string& name, float value)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform1f(location, value);
	}

	// Sets a vec2 uniform by name from two components.
	void ShaderProgram::SetUniformVec2(const std::string& name, float a, float b)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform2f(location, a, b);
	}

	// Sets a vec2 uniform by name.
	void ShaderProgram::SetUniformVec2(const std::string& name, const glm::vec2& value)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform2fv(location, 1, glm::value_ptr(value));
	}

	// Sets a vec3 uniform by name from three components.
	void ShaderProgram::SetUniformVec3(const std::string& name, float a, float b, float c)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform3f(location, a, b, c);
	}

	// Sets a vec3 uniform by name.
	void ShaderProgram::SetUniformVec3(const std::string& name, const glm::vec3& value)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform3fv(location, 1, glm::value_ptr(value));
	}

	// Sets a vec4 uniform by name from four components.
	void ShaderProgram::SetUniformVec4(const std::string& name, float a, float b, float c, float d)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform4f(location, a, b, c, d);
	}

	// Sets a vec4 uniform by name.
	void ShaderProgram::SetUniformVec4(const std::string& name, const glm::vec4& value)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniform4fv(location, 1, glm::value_ptr(value));
	}

	// Sets a mat4 uniform by name.
	void ShaderProgram::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		int location = UniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
