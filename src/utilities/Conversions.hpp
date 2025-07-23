#pragma once

#include "Config.hpp"
#include "Core.hpp"
#include "Style.hpp"

#include <hello_imgui/hello_imgui.h>
#include <imgui.h>
#include <imgui_node_editor.h>

FLOW_UI_SUBNAMESPACE_BEGIN(utility)

using namespace ax;
namespace ed = ax::NodeEditor;

constexpr ImColor to_ImColor(const Colour& c) noexcept { return ImColor(c.R, c.G, c.B, c.A); }
constexpr Colour to_Colour(const ImVec4& c) noexcept
{
    return Colour(static_cast<std::uint8_t>(c.x * 255.f), static_cast<std::uint8_t>(c.y * 255.f),
                  static_cast<std::uint8_t>(c.z * 255.f), static_cast<std::uint8_t>(c.w * 255.f));
}

FLOW_UI_SUBNAMESPACE_END
