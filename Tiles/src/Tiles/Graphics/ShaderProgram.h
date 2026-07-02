#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Core/Base.h"

namespace Tiles
{
    /// Linked vertex+fragment GL program; RAII wrapper owning the program and
    /// its two shader objects. Active uniform locations are cached at link time.
    class ShaderProgram
    {
    public:
        static std::shared_ptr<ShaderProgram> Create(const std::string& vertexSource, const std::string& fragmentSource);

        /// Compiles both stages from GLSL source, links them, and caches the
        /// locations of all active uniforms.
        ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
        ~ShaderProgram();

        void Bind() const ;
        void Unbind() const ;

        int GetAttributeLocation(const std::string& name);

        void SetUniformInt(const std::string& name, int value);
        void SetUniformFloat(const std::string& name, float value);

        void SetUniformVec2(const std::string& name, float a, float b);
        void SetUniformVec2(const std::string& name, const glm::vec2& value);

        void SetUniformVec3(const std::string& name, float a, float b, float c);
        void SetUniformVec3(const std::string& name, const glm::vec3& value);

        void SetUniformVec4(const std::string& name, float a, float b, float c, float d);
        void SetUniformVec4(const std::string& name, const glm::vec4& value);

        void SetUniformMat4(const std::string& name, const glm::mat4& value);

        /// Compiles one shader stage (@p type is GL_VERTEX_SHADER/GL_FRAGMENT_SHADER)
        /// from source, logging and returning 0 on failure.
        unsigned int CompileSource(unsigned int type, const std::string& source);
        /// Logs an error if @p name is not a known uniform; used to guard the
        /// SetUniform* helpers in development.
        void AssertUniform(const std::string& name);

		uint32_t GetID() const { return m_ShaderProgramID; }
    private:
        unsigned int m_VertexShaderID = 0;
        unsigned int m_FragmentShaderID = 0;
        unsigned int m_ShaderProgramID = 0;

        std::unordered_map<std::string, int> m_Uniforms;
    };
}
