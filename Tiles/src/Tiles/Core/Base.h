#pragma once

#include <memory>

// Yes, I could have used the definition from Lumina, but I did not want to continue to navigate the namespace. It's not worth the time.
namespace Tiles
{

    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr Shared<T> MakeShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Unique = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr Unique<T> MakeUnique(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

}