#pragma once

#include <string>
#include <memory>

namespace Tiles::Services
{
    // Command interface for undo/redo operations
    class ICommand
    {
    public:
        virtual ~ICommand() = default;

        // Execute the command
        virtual void Execute() = 0;

        // Undo the command
        virtual void Undo() = 0;

        // Get description for UI display
        virtual std::string GetDescription() const = 0;

        // Whether this command can be merged with the previous command
        // (e.g., continuous painting strokes)
        virtual bool CanMergeWith(const ICommand& other) const { return false; }

        // Merge with another command (if CanMergeWith returns true)
        virtual void MergeWith(const ICommand& other) {}
    };

    using CommandPtr = std::unique_ptr<ICommand>;
}
