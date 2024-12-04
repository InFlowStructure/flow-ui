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

constexpr ImGuiCol_ to_ImGuiCol(const Style::BaseColours& c)
{
    switch (c)
    {
    case Style::BaseColours::Border:
        return ImGuiCol_Border;
    case Style::BaseColours::BorderShadow:
        return ImGuiCol_BorderShadow;
    case Style::BaseColours::Button:
        return ImGuiCol_Button;
    case Style::BaseColours::ButtonActive:
        return ImGuiCol_ButtonActive;
    case Style::BaseColours::ButtonHovered:
        return ImGuiCol_ButtonHovered;
    case Style::BaseColours::CheckMark:
        return ImGuiCol_CheckMark;
    case Style::BaseColours::ChildBg:
        return ImGuiCol_ChildBg;
    case Style::BaseColours::DockingEmptyBg:
        return ImGuiCol_DockingEmptyBg;
    case Style::BaseColours::DockingPreview:
        return ImGuiCol_DockingPreview;
    case Style::BaseColours::DragDropTarget:
        return ImGuiCol_DragDropTarget;
    case Style::BaseColours::FrameBg:
        return ImGuiCol_FrameBg;
    case Style::BaseColours::FrameBgActive:
        return ImGuiCol_FrameBgActive;
    case Style::BaseColours::FrameBgHovered:
        return ImGuiCol_FrameBgHovered;
    case Style::BaseColours::Header:
        return ImGuiCol_Header;
    case Style::BaseColours::HeaderActive:
        return ImGuiCol_HeaderActive;
    case Style::BaseColours::HeaderHovered:
        return ImGuiCol_HeaderHovered;
    case Style::BaseColours::MenuBarBg:
        return ImGuiCol_MenuBarBg;
    case Style::BaseColours::ModalWindowDimBg:
        return ImGuiCol_ModalWindowDimBg;
    case Style::BaseColours::NavCursor:
        return ImGuiCol_NavCursor;
    case Style::BaseColours::NavWindowingDimBg:
        return ImGuiCol_NavWindowingDimBg;
    case Style::BaseColours::NavWindowingHighlight:
        return ImGuiCol_NavWindowingHighlight;
    case Style::BaseColours::PlotHistogram:
        return ImGuiCol_PlotHistogram;
    case Style::BaseColours::PlotHistogramHovered:
        return ImGuiCol_PlotHistogramHovered;
    case Style::BaseColours::PlotLines:
        return ImGuiCol_PlotLines;
    case Style::BaseColours::PlotLinesHovered:
        return ImGuiCol_PlotLinesHovered;
    case Style::BaseColours::PopupBg:
        return ImGuiCol_PopupBg;
    case Style::BaseColours::ResizeGrip:
        return ImGuiCol_ResizeGrip;
    case Style::BaseColours::ResizeGripActive:
        return ImGuiCol_ResizeGripActive;
    case Style::BaseColours::ResizeGripHovered:
        return ImGuiCol_ResizeGripHovered;
    case Style::BaseColours::ScrollbarBg:
        return ImGuiCol_ScrollbarBg;
    case Style::BaseColours::ScrollbarGrab:
        return ImGuiCol_ScrollbarGrab;
    case Style::BaseColours::ScrollbarGrabActive:
        return ImGuiCol_ScrollbarGrabActive;
    case Style::BaseColours::ScrollbarGrabHovered:
        return ImGuiCol_ScrollbarGrabHovered;
    case Style::BaseColours::Separator:
        return ImGuiCol_Separator;
    case Style::BaseColours::SeparatorActive:
        return ImGuiCol_SeparatorActive;
    case Style::BaseColours::SeparatorHovered:
        return ImGuiCol_SeparatorHovered;
    case Style::BaseColours::SliderGrab:
        return ImGuiCol_SliderGrab;
    case Style::BaseColours::SliderGrabActive:
        return ImGuiCol_SliderGrabActive;
    case Style::BaseColours::Tab:
        return ImGuiCol_Tab;
    case Style::BaseColours::TabDimmed:
        return ImGuiCol_TabDimmed;
    case Style::BaseColours::TabDimmedSelected:
        return ImGuiCol_TabDimmedSelected;
    case Style::BaseColours::TabDimmedSelectedOverline:
        return ImGuiCol_TabDimmedSelectedOverline;
    case Style::BaseColours::TabHovered:
        return ImGuiCol_TabHovered;
    case Style::BaseColours::TableBorderLight:
        return ImGuiCol_TableBorderLight;
    case Style::BaseColours::TableBorderStrong:
        return ImGuiCol_TableBorderStrong;
    case Style::BaseColours::TableHeaderBg:
        return ImGuiCol_TableHeaderBg;
    case Style::BaseColours::TableRowBg:
        return ImGuiCol_TableRowBg;
    case Style::BaseColours::TableRowBgAlt:
        return ImGuiCol_TableRowBgAlt;
    case Style::BaseColours::TabSelected:
        return ImGuiCol_TabSelected;
    case Style::BaseColours::TabSelectedOverline:
        return ImGuiCol_TabSelectedOverline;
    case Style::BaseColours::Text:
        return ImGuiCol_Text;
    case Style::BaseColours::TextDisabled:
        return ImGuiCol_TextDisabled;
    case Style::BaseColours::TextLink:
        return ImGuiCol_TextLink;
    case Style::BaseColours::TextSelectedBg:
        return ImGuiCol_TextSelectedBg;
    case Style::BaseColours::TitleBg:
        return ImGuiCol_TitleBg;
    case Style::BaseColours::TitleBgActive:
        return ImGuiCol_TitleBgActive;
    case Style::BaseColours::TitleBgCollapsed:
        return ImGuiCol_TitleBgCollapsed;
    case Style::BaseColours::WindowBg:
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
