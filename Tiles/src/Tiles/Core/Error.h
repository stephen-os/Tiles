#pragma once

#include <cstdint>
#include <string>

namespace Tiles
{
	// Categorizes a failure so callers can branch on the kind while still showing
	// the human-readable message.
	enum class ErrorCode : uint8_t
	{
		Unknown = 0,
		NoProject,     // no project is currently loaded
		NoFilePath,    // the project has no file path yet (use Save As)
		InvalidPath,   // the given path is empty or otherwise invalid
		FileNotFound,  // the file does not exist
		ReadFailure,   // the file could not be read or parsed
		WriteFailure,  // the file could not be written or encoded
		ResourceCreationFailure,  // a GPU resource (buffer, shader, texture, ...) could not be created
	};

	// The one project-wide error type carried on the failure side of a
	// std::expected: a code to branch on plus a message to display.
	struct Error
	{
		ErrorCode code = ErrorCode::Unknown;
		std::string message;
	};
}
