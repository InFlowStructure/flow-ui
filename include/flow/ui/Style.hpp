// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"
#include "utilities/Widgets.hpp"

#include <flow/core/TypeName.hpp>
#include <imgui.h>

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_START

struct Colour
{
  public:
    constexpr Colour() noexcept = default;
    constexpr Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) : R(r), G(g), B(b) {}
    constexpr Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : R(r), G(g), B(b), A(a) {}

  public:
    std::uint8_t R = 255;
    std::uint8_t G = 255;
    std::uint8_t B = 255;
    std::uint8_t A = 255;
};

struct Style
{
  public:
    enum class BaseColour : std::uint8_t
    {
        Border,
        BorderShadow,
        Button,
        ButtonActive,
        ButtonHovered,
        CheckMark,
        ChildBg,
        DockingEmptyBg,
        DockingPreview,
        DragDropTarget,
        FrameBg,
        FrameBgActive,
        FrameBgHovered,
        Header,
        HeaderActive,
        HeaderHovered,
        MenuBarBg,
        ModalWindowDimBg,
        NavCursor,
        NavWindowingDimBg,
        NavWindowingHighlight,
        PlotHistogram,
        PlotHistogramHovered,
        PlotLines,
        PlotLinesHovered,
        PopupBg,
        ResizeGrip,
        ResizeGripActive,
        ResizeGripHovered,
        ScrollbarBg,
        ScrollbarGrab,
        ScrollbarGrabActive,
        ScrollbarGrabHovered,
        Separator,
        SeparatorActive,
        SeparatorHovered,
        SliderGrab,
        SliderGrabActive,
        Tab,
        TabDimmed,
        TabDimmedSelected,
        TabDimmedSelectedOverline,
        TabHovered,
        TableBorderLight,
        TableBorderStrong,
        TableHeaderBg,
        TableRowBg,
        TableRowBgAlt,
        TabSelected,
        TabSelectedOverline,
        Text,
        TextDisabled,
        TextLink,
        TextSelectedBg,
        TitleBg,
        TitleBgActive,
        TitleBgCollapsed,
        WindowBg,
    };

    enum class NodeEditorColours : std::uint8_t
    {
        Bg,
        Flow,
        FlowMarker,
        Grid,
        GroupBg,
        GroupBorder,
        HighlightLinkBorder,
        HovLinkBorder,
        HovNodeBorder,
        LinkSelRect,
        LinkSelRectBorder,
        NodeBg,
        NodeBorder,
        NodeSelRect,
        NodeSelRectBorder,
        PinRect,
        PinRectBorder,
        SelLinkBorder,
        SelNodeBorder,
    };

  public:
    Style();

    void SetTypeColour(std::string_view type, const Colour& colour);
    Colour GetTypeColour(std::string_view type) const;

    template<typename T>
    void SetTypeColour(const Colour& colour)
    {
        return SetTypeColour(flow::TypeName_v<T>, colour);
    }

    template<typename T>
    Colour GetTypeColour() const
    {
        return GetTypeColour(flow::TypeName_v<T>);
    }

  public:
    struct
    {
        widgets::IconType Default;
        widgets::IconType Ref;
    } PortShapes;

    struct
    {
        std::map<BaseColour, Colour> BaseColours;
        std::map<NodeEditorColours, Colour> NodeEdtiorColours;
        std::unordered_map<std::string_view, Colour> TypeColours;
    } Colours;

    float CircleTessellationMaxError;
    float CurveTessellationTol;
    float WindowBorderSize;
    float FrameBorderSize;
    float TabRounding;
    float TabBarBorderSize;
    struct
    {
        float Width;
        float Height;
    } CellPadding;
};

Style& GetStyle();

FLOW_UI_NAMESPACE_END
