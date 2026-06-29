#pragma once

#include "../Entities/TileProject.h"

#include <memory>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>

namespace Tiles::Domain
{
    // Result type for repository operations
    struct RepositoryError
    {
        enum class Code
        {
            FileNotFound,
            AccessDenied,
            InvalidFormat,
            IOError,
            Unknown
        };

        Code ErrorCode;
        std::string Message;
    };

    // C++17 compatible Result type (similar to std::expected)
    template<typename T>
    class RepositoryResult
    {
    public:
        RepositoryResult(T value) : m_Data(std::move(value)) {}
        RepositoryResult(RepositoryError error) : m_Data(std::move(error)) {}

        bool has_value() const { return std::holds_alternative<T>(m_Data); }
        explicit operator bool() const { return has_value(); }

        T& value() { return std::get<T>(m_Data); }
        const T& value() const { return std::get<T>(m_Data); }

        RepositoryError& error() { return std::get<RepositoryError>(m_Data); }
        const RepositoryError& error() const { return std::get<RepositoryError>(m_Data); }

    private:
        std::variant<T, RepositoryError> m_Data;
    };

    // Specialization for void
    template<>
    class RepositoryResult<void>
    {
    public:
        RepositoryResult() : m_Error(std::nullopt) {}
        RepositoryResult(RepositoryError error) : m_Error(std::move(error)) {}

        bool has_value() const { return !m_Error.has_value(); }
        explicit operator bool() const { return has_value(); }

        RepositoryError& error() { return m_Error.value(); }
        const RepositoryError& error() const { return m_Error.value(); }

    private:
        std::optional<RepositoryError> m_Error;
    };

    // Repository interface for project persistence
    // Implemented in Infrastructure layer
    class IProjectRepository
    {
    public:
        virtual ~IProjectRepository() = default;

        // Save project to file
        virtual RepositoryResult<void> Save(const TileProject& project, const std::filesystem::path& path) = 0;

        // Load project from file
        virtual RepositoryResult<std::unique_ptr<TileProject>> Load(const std::filesystem::path& path) = 0;

        // Check if file exists
        virtual bool Exists(const std::filesystem::path& path) const = 0;

        // Get last modified time of file
        virtual std::optional<std::filesystem::file_time_type> GetLastModified(const std::filesystem::path& path) const = 0;
    };
}
