// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Style.hpp"

#include <any>
#include <cstdint>

FLOW_UI_NAMESPACE_START
static Style style{
    .PortShapes{
        .Default = widgets::IconType::Circle,
        .Ref     = widgets::IconType::Diamond,
    },
    .PortColours{
        {std::string{TypeName_v<std::any>}, ImColor(120, 120, 127)},

        {std::string{"flow::Nodes::ReflectType"}, ImColor(6, 68, 154)},
        {std::string{"flow::EnumAsByte"}, ImColor(0, 109, 99)},

        {std::string{TypeName_v<bool>}, ImColor(220, 48, 48)},

        {std::string{TypeName_v<std::int8_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::int16_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::int32_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::int64_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::uint8_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::uint16_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::uint32_t>}, ImColor(68, 201, 156)},
        {std::string{TypeName_v<std::uint64_t>}, ImColor(68, 201, 156)},

        {std::string{TypeName_v<float>}, ImColor(147, 226, 74)},
        {std::string{TypeName_v<double>}, ImColor(147, 226, 74)},

        {std::string{TypeName_v<std::string>}, ImColor(124, 21, 153)},
    },
};

Style& GetStyle() { return style; }

void Style::SetPortColour(std::string_view type, ImVec4 colour) { PortColours[std::string{type}] = std::move(colour); }

ImColor Style::GetPortColour(std::string_view type) const
{
    if (PortColours.contains(std::string{type}))
    {
        return PortColours.at(std::string{type});
    }

    for (const auto& [port_type, colour] : PortColours)
    {
        if (type.find(port_type) != std::string_view::npos)
        {
            return colour;
        }
    }

    return ImColor(255, 255, 255);
}
FLOW_UI_NAMESPACE_END
