#pragma once

#include "Core.hpp"
#include "Style.hpp"

#include <imgui.h>
#include <imgui_node_editor.h>

FLOW_UI_SUBNAMESPACE_START(utility)

using namespace ax;
namespace ed = ax::NodeEditor;

constexpr ed::StyleColor to_EdStyleColour(const Style::NodeEditorColours& c)
{
    switch (c)
    {
    case Style::NodeEditorColours::Bg:
        return ed::StyleColor_Bg;
    case Style::NodeEditorColours::Flow:
        return ed::StyleColor_Flow;
    case Style::NodeEditorColours::FlowMarker:
        return ed::StyleColor_FlowMarker;
    case Style::NodeEditorColours::Grid:
        return ed::StyleColor_Grid;
    case Style::NodeEditorColours::GroupBg:
        return ed::StyleColor_GroupBg;
    case Style::NodeEditorColours::GroupBorder:
        return ed::StyleColor_GroupBorder;
    case Style::NodeEditorColours::HighlightLinkBorder:
        return ed::StyleColor_HighlightLinkBorder;
    case Style::NodeEditorColours::HovLinkBorder:
        return ed::StyleColor_HovLinkBorder;
    case Style::NodeEditorColours::HovNodeBorder:
        return ed::StyleColor_HovNodeBorder;
    case Style::NodeEditorColours::LinkSelRect:
        return ed::StyleColor_LinkSelRect;
    case Style::NodeEditorColours::LinkSelRectBorder:
        return ed::StyleColor_LinkSelRectBorder;
    case Style::NodeEditorColours::NodeBg:
        return ed::StyleColor_NodeBg;
    case Style::NodeEditorColours::NodeBorder:
        return ed::StyleColor_NodeBorder;
    case Style::NodeEditorColours::NodeSelRect:
        return ed::StyleColor_NodeSelRect;
    case Style::NodeEditorColours::NodeSelRectBorder:
        return ed::StyleColor_NodeSelRectBorder;
    case Style::NodeEditorColours::PinRect:
        return ed::StyleColor_PinRect;
    case Style::NodeEditorColours::PinRectBorder:
        return ed::StyleColor_PinRectBorder;
    case Style::NodeEditorColours::SelLinkBorder:
        return ed::StyleColor_SelLinkBorder;
    case Style::NodeEditorColours::SelNodeBorder:
        return ed::StyleColor_SelNodeBorder;
    }

    return ed::StyleColor(0);
}
constexpr Style::NodeEditorColours to_NodeEditorColours(const ed::StyleColor& c)
{
    switch (c)
    {
    case ed::StyleColor_Bg:
        return Style::NodeEditorColours::Bg;
    case ed::StyleColor_Flow:
        return Style::NodeEditorColours::Flow;
    case ed::StyleColor_FlowMarker:
        return Style::NodeEditorColours::FlowMarker;
    case ed::StyleColor_Grid:
        return Style::NodeEditorColours::Grid;
    case ed::StyleColor_GroupBg:
        return Style::NodeEditorColours::GroupBg;
    case ed::StyleColor_GroupBorder:
        return Style::NodeEditorColours::GroupBorder;
    case ed::StyleColor_HighlightLinkBorder:
        return Style::NodeEditorColours::HighlightLinkBorder;
    case ed::StyleColor_HovLinkBorder:
        return Style::NodeEditorColours::HovLinkBorder;
    case ed::StyleColor_HovNodeBorder:
        return Style::NodeEditorColours::HovNodeBorder;
    case ed::StyleColor_LinkSelRect:
        return Style::NodeEditorColours::LinkSelRect;
    case ed::StyleColor_LinkSelRectBorder:
        return Style::NodeEditorColours::LinkSelRectBorder;
    case ed::StyleColor_NodeBg:
        return Style::NodeEditorColours::NodeBg;
    case ed::StyleColor_NodeBorder:
        return Style::NodeEditorColours::NodeBorder;
    case ed::StyleColor_NodeSelRect:
        return Style::NodeEditorColours::NodeSelRect;
    case ed::StyleColor_NodeSelRectBorder:
        return Style::NodeEditorColours::NodeSelRectBorder;
    case ed::StyleColor_PinRect:
        return Style::NodeEditorColours::PinRect;
    case ed::StyleColor_PinRectBorder:
        return Style::NodeEditorColours::PinRectBorder;
    case ed::StyleColor_SelLinkBorder:
        return Style::NodeEditorColours::SelLinkBorder;
    case ed::StyleColor_SelNodeBorder:
        return Style::NodeEditorColours::SelNodeBorder;
    default:
        return Style::NodeEditorColours(0);
    }

    return Style::NodeEditorColours(0);
}

constexpr ImGuiCol_ to_ImGuiCol(const Style::BaseColour& c)
{
    switch (c)
    {
    case Style::BaseColour::Border:
        return ImGuiCol_Border;
    case Style::BaseColour::BorderShadow:
        return ImGuiCol_BorderShadow;
    case Style::BaseColour::Button:
        return ImGuiCol_Button;
    case Style::BaseColour::ButtonActive:
        return ImGuiCol_ButtonActive;
    case Style::BaseColour::ButtonHovered:
        return ImGuiCol_ButtonHovered;
    case Style::BaseColour::CheckMark:
        return ImGuiCol_CheckMark;
    case Style::BaseColour::ChildBg:
        return ImGuiCol_ChildBg;
    case Style::BaseColour::DockingEmptyBg:
        return ImGuiCol_DockingEmptyBg;
    case Style::BaseColour::DockingPreview:
        return ImGuiCol_DockingPreview;
    case Style::BaseColour::DragDropTarget:
        return ImGuiCol_DragDropTarget;
    case Style::BaseColour::FrameBg:
        return ImGuiCol_FrameBg;
    case Style::BaseColour::FrameBgActive:
        return ImGuiCol_FrameBgActive;
    case Style::BaseColour::FrameBgHovered:
        return ImGuiCol_FrameBgHovered;
    case Style::BaseColour::Header:
        return ImGuiCol_Header;
    case Style::BaseColour::HeaderActive:
        return ImGuiCol_HeaderActive;
    case Style::BaseColour::HeaderHovered:
        return ImGuiCol_HeaderHovered;
    case Style::BaseColour::MenuBarBg:
        return ImGuiCol_MenuBarBg;
    case Style::BaseColour::ModalWindowDimBg:
        return ImGuiCol_ModalWindowDimBg;
    case Style::BaseColour::NavCursor:
        return ImGuiCol_NavCursor;
    case Style::BaseColour::NavWindowingDimBg:
        return ImGuiCol_NavWindowingDimBg;
    case Style::BaseColour::NavWindowingHighlight:
        return ImGuiCol_NavWindowingHighlight;
    case Style::BaseColour::PlotHistogram:
        return ImGuiCol_PlotHistogram;
    case Style::BaseColour::PlotHistogramHovered:
        return ImGuiCol_PlotHistogramHovered;
    case Style::BaseColour::PlotLines:
        return ImGuiCol_PlotLines;
    case Style::BaseColour::PlotLinesHovered:
        return ImGuiCol_PlotLinesHovered;
    case Style::BaseColour::PopupBg:
        return ImGuiCol_PopupBg;
    case Style::BaseColour::ResizeGrip:
        return ImGuiCol_ResizeGrip;
    case Style::BaseColour::ResizeGripActive:
        return ImGuiCol_ResizeGripActive;
    case Style::BaseColour::ResizeGripHovered:
        return ImGuiCol_ResizeGripHovered;
    case Style::BaseColour::ScrollbarBg:
        return ImGuiCol_ScrollbarBg;
    case Style::BaseColour::ScrollbarGrab:
        return ImGuiCol_ScrollbarGrab;
    case Style::BaseColour::ScrollbarGrabActive:
        return ImGuiCol_ScrollbarGrabActive;
    case Style::BaseColour::ScrollbarGrabHovered:
        return ImGuiCol_ScrollbarGrabHovered;
    case Style::BaseColour::Separator:
        return ImGuiCol_Separator;
    case Style::BaseColour::SeparatorActive:
        return ImGuiCol_SeparatorActive;
    case Style::BaseColour::SeparatorHovered:
        return ImGuiCol_SeparatorHovered;
    case Style::BaseColour::SliderGrab:
        return ImGuiCol_SliderGrab;
    case Style::BaseColour::SliderGrabActive:
        return ImGuiCol_SliderGrabActive;
    case Style::BaseColour::Tab:
        return ImGuiCol_Tab;
    case Style::BaseColour::TabDimmed:
        return ImGuiCol_TabDimmed;
    case Style::BaseColour::TabDimmedSelected:
        return ImGuiCol_TabDimmedSelected;
    case Style::BaseColour::TabDimmedSelectedOverline:
        return ImGuiCol_TabDimmedSelectedOverline;
    case Style::BaseColour::TabHovered:
        return ImGuiCol_TabHovered;
    case Style::BaseColour::TableBorderLight:
        return ImGuiCol_TableBorderLight;
    case Style::BaseColour::TableBorderStrong:
        return ImGuiCol_TableBorderStrong;
    case Style::BaseColour::TableHeaderBg:
        return ImGuiCol_TableHeaderBg;
    case Style::BaseColour::TableRowBg:
        return ImGuiCol_TableRowBg;
    case Style::BaseColour::TableRowBgAlt:
        return ImGuiCol_TableRowBgAlt;
    case Style::BaseColour::TabSelected:
        return ImGuiCol_TabSelected;
    case Style::BaseColour::TabSelectedOverline:
        return ImGuiCol_TabSelectedOverline;
    case Style::BaseColour::Text:
        return ImGuiCol_Text;
    case Style::BaseColour::TextDisabled:
        return ImGuiCol_TextDisabled;
    case Style::BaseColour::TextLink:
        return ImGuiCol_TextLink;
    case Style::BaseColour::TextSelectedBg:
        return ImGuiCol_TextSelectedBg;
    case Style::BaseColour::TitleBg:
        return ImGuiCol_TitleBg;
    case Style::BaseColour::TitleBgActive:
        return ImGuiCol_TitleBgActive;
    case Style::BaseColour::TitleBgCollapsed:
        return ImGuiCol_TitleBgCollapsed;
    case Style::BaseColour::WindowBg:
        return ImGuiCol_WindowBg;
    }

    return ImGuiCol_(0);
}

constexpr ImColor to_ImColor(const Colour& c) noexcept { return ImColor(c.R, c.G, c.B, c.A); }
constexpr Colour to_Colour(const ImVec4& c) noexcept
{
    return Colour(c.x * 255.f, c.y * 255.f, c.z * 255.f, c.w * 255.f);
}

inline ImGuiStyle& to_ImGuiStyle(const Style& style) noexcept
{
    auto& imgui_style                      = ImGui::GetStyle();
    imgui_style.CircleTessellationMaxError = style.CircleTessellationMaxError;
    imgui_style.CurveTessellationTol       = style.CurveTessellationTol;
    imgui_style.WindowBorderSize           = style.WindowBorderSize;
    imgui_style.FrameBorderSize            = style.FrameBorderSize;
    imgui_style.TabRounding                = style.TabRounding;
    imgui_style.TabBarBorderSize           = style.TabBarBorderSize;
    imgui_style.CellPadding                = ImVec2(style.CellPadding.Width, style.CellPadding.Height);

    auto& imgui_colours = imgui_style.Colors;
    for (auto& [type, colour] : style.Colours.BaseColours)
    {
        imgui_colours[to_ImGuiCol(type)] = to_ImColor(colour);
    }

    return imgui_style;
}

FLOW_UI_SUBNAMESPACE_END
