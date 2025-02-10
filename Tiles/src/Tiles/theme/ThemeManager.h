#pragma once

#include "imgui.h"

class ThemeManager 
{
public:
    static ThemeManager& GetInstance();

    void ApplyDarkTheme();

private:
    ThemeManager() = default;
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
};
