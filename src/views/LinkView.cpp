// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "LinkView.hpp"
#include "utilities/Conversions.hpp"

#include <stdexcept>

FLOW_UI_NAMESPACE_START

Link::Link(const flow::UUID& connection_id, ed::PinId startPinId, ed::PinId endPinId, const Colour& colour)
    : ID(std::hash<flow::UUID>{}(connection_id)), StartPinID(startPinId), EndPinID(endPinId), _colour(colour)
{
}

void Link::Draw()
{
    if (!ed::Link(ID, StartPinID, EndPinID, utility::to_ImColor(_colour), 2.0f))
    {
        throw std::runtime_error("Failed to set link for pins");
    }

    if (ed::IsLinkSelected(ID) && ImGui::IsMouseDoubleClicked(0))
    {
        // TODO: Reroute nodes
    }

    if (IsFlowing())
    {
        ed::Flow(ID);
        SetFlowing(false);
    }
}

FLOW_UI_NAMESPACE_END
