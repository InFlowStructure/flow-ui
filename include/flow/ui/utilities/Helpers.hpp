// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <imgui_node_editor.h>

#include <functional>

namespace ax::NodeEditor::Detail
{
struct EditorContext;
}

template<>
struct std::less<ax::NodeEditor::NodeId>
{
    bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

template<>
struct std::less<ax::NodeEditor::LinkId>
{
    bool operator()(const ax::NodeEditor::LinkId& lhs, const ax::NodeEditor::LinkId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

template<>
struct std::less<ax::NodeEditor::PinId>
{
    bool operator()(const ax::NodeEditor::PinId& lhs, const ax::NodeEditor::PinId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

template<>
struct std::hash<ax::NodeEditor::NodeId>
{
    size_t operator()(const ax::NodeEditor::NodeId& value) const { return std::hash<void*>{}(value.AsPointer()); }
};

template<>
struct std::hash<ax::NodeEditor::LinkId>
{
    size_t operator()(const ax::NodeEditor::LinkId& value) const { return std::hash<void*>{}(value.AsPointer()); }
};
