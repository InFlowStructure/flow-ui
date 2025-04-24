#pragma once

#include "Core.hpp"

#include <nlohmann/json.hpp>

#include <string>

FLOW_UI_NAMESPACE_START

struct ModuleInfo
{
    std::string Name;
    std::string Version;
    std::string Author;
    std::string Description;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModuleInfo, Name, Version, Author, Description);

FLOW_UI_NAMESPACE_END
