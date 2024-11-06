// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <hello_imgui/hello_imgui.h>

#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_START

struct Config
{
    ImFont* DefaultFont                           = nullptr;
    ImFont* IconFont                              = nullptr;
    ImFont* NodeHeaderFont                        = nullptr;
    HelloImGui::RendererBackendType RenderBackend = HelloImGui::RendererBackendType::FirstAvailable;
};

ImFont* LoadFont(const std::string& font_path, float size, const HelloImGui::FontLoadingParams& params);

Config& GetConfig();

FLOW_UI_NAMESPACE_END
