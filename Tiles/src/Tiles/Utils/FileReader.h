#pragma once

#include "Core/Error.h"

#include <expected>
#include <fstream>
#include <string>

namespace Tiles
{
    // Maximum file size we'll load (64MB - reasonable for shader/config files)
    inline constexpr size_t MaxFileSize = 64 * 1024 * 1024;

    // Reads an entire file into a string.
    // @param filename Path to the file to read.
    // @param maxSize Upper bound on file size; larger files are rejected.
    // @return The file's raw bytes, or an Error if it cannot be opened, sized,
    //         read, or exceeds maxSize.
    [[nodiscard]] inline std::expected<std::string, Error> ReadFile(const std::string& filename,
                                                                    size_t maxSize = MaxFileSize) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            return std::unexpected(Error{ ErrorCode::FileNotFound, "Failed to open file: " + filename });

        std::streampos fileSize = file.tellg();
        if (fileSize < 0)
            return std::unexpected(Error{ ErrorCode::ReadFailure, "Failed to determine file size: " + filename });

        auto size = static_cast<size_t>(fileSize);
        if (size > maxSize)
            return std::unexpected(Error{ ErrorCode::ReadFailure,
                "File exceeds maximum allowed size (" + std::to_string(maxSize) + " bytes): " + filename });

        std::string buffer;
        buffer.resize(size);

        file.seekg(0);
        if (!file.read(buffer.data(), static_cast<std::streamsize>(size)))
            return std::unexpected(Error{ ErrorCode::ReadFailure, "Failed to read file contents: " + filename });

        return buffer;
    }
}
