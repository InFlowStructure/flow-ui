// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Config.hpp"

#include <hello_imgui/hello_imgui.h>

#include <bit>
#include <cstdint>

FLOW_UI_NAMESPACE_START

static Config config{};

std::unique_ptr<Font> LoadFont(const std::string& font_path, float size)
{
    HelloImGui::FontLoadingParams params;
    params.useFullGlyphRange                 = true;
    params.fontConfig.RasterizerDensity      = 4.f;
    params.reduceMemoryUsageIfFullGlyphRange = true;
    auto font                                = HelloImGui::LoadFont(font_path, size, params);

    return std::unique_ptr<Font>(std::bit_cast<Font*>(font));
}

Config& GetConfig() { return config; }

FLOW_UI_NAMESPACE_END
