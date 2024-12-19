// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "ConnectionView.hpp"
#include "utilities/Conversions.hpp"

#include <imgui_node_editor.h>

#include <stdexcept>

FLOW_UI_NAMESPACE_START

namespace ed = ax::NodeEditor;

ConnectionView::ConnectionView(const flow::UUID& id, std::uint64_t start_port_id, std::uint64_t end_port_id,
                               const Colour& colour)
    : ID(std::hash<flow::UUID>{}(id)), StartPortID(start_port_id), EndPortID(end_port_id), _colour(colour)
{
}

void ConnectionView::Draw()
{
    if (!ed::Link(ID, StartPortID, EndPortID, utility::to_ImColor(_colour), 2.0f))
    {
        throw std::runtime_error("Failed to set link for pins");
    }

    if (ed::IsLinkSelected(ID) && ImGui::IsMouseDoubleClicked(0))
    {
        // TODO(trigaux): Reroute nodes
    }

    if (IsFlowing())
    {
        ed::Flow(ID);
        SetFlowing(false);
    }
}

FLOW_UI_NAMESPACE_END
