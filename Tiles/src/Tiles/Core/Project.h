#pragma once

#include "Layers.h"
#include "Atlas.h"
#include "Base.h"

#include "json.hpp"

#include <fstream>

namespace Tiles
{

    class Project
    {
    public:
        static void Save(const std::string& path, const Shared<Layers>& layers, const Shared<Atlas>& atlas);
        static void Load(const std::string& path, Shared<Layers>& layers, Shared<Atlas>& atlas);
    };

}