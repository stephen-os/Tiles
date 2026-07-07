#pragma once

#include <string>
#include <fstream>
#include <stdexcept>

namespace Tiles
{
    // Maximum file size we'll load (64MB - reasonable for shader/config files)
    inline constexpr size_t MaxFileSize = 64 * 1024 * 1024;

    struct FileReadError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    // Reads an entire file into a string.
    // @param filename Path to the file to read.
    // @param maxSize Upper bound on file size; larger files are rejected.
    // @return The file's raw bytes.
    // @throws FileReadError if the file cannot be opened, sized, read, or exceeds maxSize.
    [[nodiscard]] inline std::string ReadFile(const std::string& filename,
                                              size_t maxSize = MaxFileSize) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw FileReadError("Failed to open file: " + filename);
        }

        std::streampos fileSize = file.tellg();
        if (fileSize < 0) {
            throw FileReadError("Failed to determine file size: " + filename);
        }

        auto size = static_cast<size_t>(fileSize);
        if (size > maxSize) {
            throw FileReadError("File exceeds maximum allowed size (" +
                               std::to_string(maxSize) + " bytes): " + filename);
        }

        std::string buffer;
        buffer.resize(size);

        file.seekg(0);
        if (!file.read(buffer.data(), static_cast<std::streamsize>(size))) {
            throw FileReadError("Failed to read file contents: " + filename);
        }

        return buffer;
    }
}
