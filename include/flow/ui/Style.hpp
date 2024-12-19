// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <flow/core/TypeName.hpp>

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_START

/**
 * @brief Enumeration of different types of Port Icons.
 */
enum class PortIconType : std::uint8_t
{
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

/**
 * @brief RGBA Colour type. Values are uint8_t.
 */
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

/**
 * @brief Universal style for the Editor.
 */
struct Style
{
  public:
    /**
     * @brief Colour flags for windows and widgets.
     */
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

    /**
     * @brief Colour flags for graph editor.
     */
    enum class EditorColour : std::uint8_t
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

    /**
     * @brief Set port colour for specific port typename.
     * @param type The name of the type.
     * @param colour The colour to render the port as.
     */
    void SetTypeColour(std::string_view type, const Colour& colour);

    /**
     * @brief Get the colour of a port type by typename.
     * @tparam T The type of the port.
     * @returns The colours registered for the given typename.
     */
    Colour GetTypeColour(std::string_view type) const;

    template<typename T>
    void SetTypeColour(const Colour& colour)
    {
        return SetTypeColour(flow::TypeName_v<T>, colour);
    }

    /**
     * @brief Get the colour of a port type by typename.
     * @tparam T The type of the port.
     * @returns The colours registered for the given type.
     */
    template<typename T>
    Colour GetTypeColour() const
    {
        return GetTypeColour(flow::TypeName_v<T>);
    }

  public:
    /// The Icon shapes for different kinds of ports.
    struct
    {
        /// Icon shape for default port type.
        PortIconType Default;

        /// Icon shape for reference port types.
        PortIconType Ref;
    } PortShapes;

    /// Registered colours for the editor.
    struct
    {
        /// Registered colours for windows and widgets.
        std::map<BaseColour, Colour> BaseColours;

        /// Registered colours for the graph editor.
        std::map<EditorColour, Colour> EditorColours;

        /// Registered colours for port types.
        std::unordered_map<std::string_view, Colour> TypeColours;
    } Colours;

    /// Window border size.
    float WindowBorderSize;

    /// Size of inner window frame borders.
    float FrameBorderSize;

    /// Rounding for windows tabs.
    float TabRounding;

    /// Border size for windows tabs.
    float TabBarBorderSize;

    /// Padding for table cells.
    struct
    {
        /// Cell padding width.
        float Width;

        /// Cell padding height.
        float Height;
    } CellPadding;
};

/**
 * @brief Get the global style for the editor.
 * @returns The global style.
 */
Style& GetStyle();

FLOW_UI_NAMESPACE_END
