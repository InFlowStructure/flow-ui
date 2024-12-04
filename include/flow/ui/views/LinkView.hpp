// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"

#include <flow/core/UUID.hpp>
#include <imgui_node_editor.h>

#include <mutex>

FLOW_UI_NAMESPACE_START
namespace ed = ax::NodeEditor;

class Link
{
  public:
    Link(const flow::UUID& connection_id, ed::PinId startPinId, ed::PinId endPinId, ImColor colour);

    void Draw();

    bool IsFlowing() const noexcept { return _flowing; }
    void SetFlowing(bool flowing) noexcept { _flowing = flowing; }

  public:
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Colour;

  private:
    bool _flowing = false;
};
FLOW_UI_NAMESPACE_END
