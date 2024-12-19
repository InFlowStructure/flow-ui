// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Style.hpp"
#include "utilities/Conversions.hpp"

#include <imgui_node_editor.h>

#include <any>
#include <cstdint>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

Style::Style()
    : PortShapes{
        .Default = PortIconType::Circle,
        .Ref     = PortIconType::Diamond,
    },
    Colours{.TypeColours{
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
    }},
    WindowBorderSize(5.f),
    FrameBorderSize(2.f),
    TabRounding(8.f),
    TabBarBorderSize(0.f),
    CellPadding{.Width = 7.f, .Height = 7.f}
{
    auto ed_style    = ed::Style{};
    auto& ed_colours = ed_style.Colors;

    int i = 0;
    std::for_each(std::begin(ed_colours), std::end(ed_colours), [&](const auto& c) {
        Colours.EditorColours[utility::to_EditorColour(ed::StyleColor(i++))] = utility::to_Colour(c);
    });
}

static Style style{};

Style& GetStyle() { return style; }

void Style::SetTypeColour(std::string_view type, const Colour& colour)
{
    Colours.TypeColours[type] = std::move(colour);
}

Colour Style::GetTypeColour(std::string_view type) const
{
    if (Colours.TypeColours.contains(type))
    {
        return Colours.TypeColours.at(type);
    }

    for (const auto& [port_type, colour] : Colours.TypeColours)
    {
        if (type.find(port_type) != std::string_view::npos)
        {
            return colour;
        }
    }

    return Colour(255, 255, 255);
}
FLOW_UI_NAMESPACE_END
