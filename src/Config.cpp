// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Config.hpp"

#include <cstdint>

FLOW_UI_NAMESPACE_START
static Config config{
    .DefaultFont    = nullptr,
    .IconFont       = nullptr,
    .NodeHeaderFont = nullptr,
};

ImFont* LoadFont(const std::string& font_path, float size, const HelloImGui::FontLoadingParams& params)
{
    return HelloImGui::LoadFont(font_path, size, params);
}

Config& GetConfig() { return config; }

FLOW_UI_NAMESPACE_END
