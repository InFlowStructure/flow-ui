// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"

#include <imgui_node_editor.h>

FLOW_UI_SUBNAMESPACE_START(utility)

namespace ed = ax::NodeEditor;

class NodeBuilder
{
  public:
    NodeBuilder() = default;

    void Begin(ed::NodeId id);
    void End();

    void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
    void EndHeader();

    void Input(ed::PinId id);
    void EndInput();

    void Middle();

    void Output(ed::PinId id);
    void EndOutput();

    const ImVec2& GetMinPos() const noexcept { return _node_min; }
    const ImVec2& GetMaxPos() const noexcept { return _node_max; }
    ImVec2 GetSize() const noexcept { return _node_max - _node_min; }

  private:
    enum class Stage
    {
        Invalid,
        Begin,
        Header,
        Content,
        Input,
        Output,
        Middle,
        End
    };

    bool SetStage(Stage stage);

    void Pin(ed::PinId id, ax::NodeEditor::PinKind kind);
    void EndPin();

  private:
    ed::NodeId _current_node_id = 0;
    bool _has_header            = false;
    Stage _current_stage        = Stage::Invalid;
    ImU32 _header_color         = ImColor(1, 1, 1, 1);
    ImVec2 _node_min;
    ImVec2 _node_max;
    ImVec2 _header_min;
    ImVec2 _header_max;
    ImVec2 _content_min;
    ImVec2 _content_max;
};

FLOW_UI_SUBNAMESPACE_END
