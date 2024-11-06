// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"
#include "utilities/Widgets.hpp"

#include <flow/core/TypeName.hpp>
#include <imgui-node-editor/imgui_node_editor.h>
#include <imgui.h>

#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

struct Style
{
    struct
    {
        widgets::IconType Default;
        widgets::IconType Ref;
    } PortShapes;

    std::unordered_map<std::string, ImVec4> PortColours;

    void SetPortColour(std::string_view type, ImVec4 colour);
    ImColor GetPortColour(std::string_view type) const;

    template<typename T>
    void SetPortColour(ImVec4 colour)
    {
        return SetPortColour(flow::TypeName_v<T>, std::move(colour));
    }

    template<typename T>
    ImColor GetPortColour() const
    {
        return GetPortColour(flow::TypeName_v<T>);
    }

    ImFont* NodeHeaderFont = nullptr;

    ImVec4 Colours[ed::StyleColor_Count];
};

Style& GetStyle();

FLOW_UI_NAMESPACE_END
