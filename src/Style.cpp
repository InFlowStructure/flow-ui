// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Style.hpp"
#include "utilities/Conversions.hpp"

#include <imgui_node_editor.h>

#include <any>
#include <cstdint>

FLOW_UI_NAMESPACE_BEGIN

using namespace ax;
namespace ed = ax::NodeEditor;

Style::Style()
    : PortShapes{
        .Default = PortIconType::Circle,
        .Ref     = PortIconType::Diamond,
    },
    TypeColours{
        {TypeName_v<std::any>, Colour(120, 120, 127)},

        {"flow::Struct", Colour(6, 68, 154)},
        {"flow::EnumAsByte", Colour(0, 109, 99)},

        {TypeName_v<bool>, Colour(220, 48, 48)},

        {TypeName_v<std::int8_t>, Colour(68, 201, 156)},
        {TypeName_v<std::int16_t>, Colour(68, 201, 156)},
        {TypeName_v<std::int32_t>, Colour(68, 201, 156)},
        {TypeName_v<std::int64_t>, Colour(68, 201, 156)},
        {TypeName_v<std::uint8_t>, Colour(68, 201, 156)},
        {TypeName_v<std::uint16_t>, Colour(68, 201, 156)},
        {TypeName_v<std::uint32_t>, Colour(68, 201, 156)},
        {TypeName_v<std::uint64_t>, Colour(68, 201, 156)},

        {TypeName_v<float>, Colour(147, 226, 74)},
        {TypeName_v<double>, Colour(147, 226, 74)},

        {TypeName_v<std::string>, Colour(124, 21, 153)},
    }
{
}

static Style style{};

Style& GetStyle() { return style; }

void Style::SetTypeColour(std::string_view type, const Colour& colour) { TypeColours[type] = std::move(colour); }

Colour Style::GetTypeColour(std::string_view type) const
{
    if (TypeColours.contains(type))
    {
        return TypeColours.at(type);
    }

    for (const auto& [port_type, colour] : TypeColours)
    {
        if (type.find(port_type) != std::string_view::npos)
        {
            return colour;
        }
    }

    return Colour(255, 255, 255);
}
FLOW_UI_NAMESPACE_END
