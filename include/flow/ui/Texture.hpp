// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <flow/core/NodeData.hpp>

#include <hello_imgui/hello_imgui.h>
#include <hello_imgui/internal/image_abstract.h>

FLOW_UI_NAMESPACE_START

struct Texture
{
    Texture(std::uint8_t* data, int width, int height);
    Texture(unsigned int texture_id, int width, int height);
    Texture(ImTextureID ID, ImVec2 Size);

    ImTextureID ID;
    ImVec2 Size;
};

FLOW_UI_NAMESPACE_END
