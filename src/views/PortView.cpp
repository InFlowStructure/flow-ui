// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "PortView.hpp"

#include "InputField.hpp"
#include "LinkView.hpp"
#include "ViewFactory.hpp"
#include "Widgets.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/NodeData.hpp>

#include <imgui_internal.h>
#include <spdlog/spdlog.h>

#include <map>
#include <vector>

FLOW_UI_NAMESPACE_START

using widgets::IconType;
namespace
{
IconType GetIconType(std::string_view type)
{
    if (type.find("vector") != std::string_view::npos) return IconType::Grid;

    if (type.find("*") != std::string_view::npos || type.find("unique_ptr") != std::string_view::npos ||
        type.find("&") != std::string_view::npos)
    {
        return GetStyle().PortShapes.Ref;
    }

    return GetStyle().PortShapes.Default;
}

void DrawPinIcon(const PortView& pin, bool connected, int alpha)
{
    auto colour    = pin.Colour();
    colour.Value.w = alpha / 255.0f;
    widgets::Icon(ImVec2(24.f, 24.f), GetIconType(pin.Type()), connected, colour, ImColor(32, 32, 32, alpha));
}
} // namespace

PortView::PortView(const ed::NodeId& node_id, std::shared_ptr<Port> port_data,
                   const std::shared_ptr<ViewFactory>& factory, OnInputFunc input_function, bool show_label)
    : ID(std::hash<flow::UUID>{}({})), NodeID(node_id), _port{std::move(port_data)},
      _show_label{_port->GetKey() != flow::IndexableName::None && show_label}, OnSetInput{input_function}
{
    const auto& input_ctors = factory->GetRegisteredInputTypes();
    if (input_ctors.contains(std::string{Type()}))
    {
        _input_field = input_ctors.at(std::string{Type()})(std::string{Name()}, _port->GetData(),
                                                           ImGuiInputTextFlags_AutoSelectAll);
    }
}

void PortView::Draw()
{
    if (Kind == PinKind::Input)
    {
        _builder->Port(ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, _alpha);
        DrawIcon(_alpha);
        DrawLabel();
        ImGui::PopStyleVar();

        if (!IsLinked())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
            DrawInput();
            ImGui::PopStyleVar();
        }

        if (ImGui::IsItemActive() && !_was_active)
        {
            ed::EnableShortcuts(false);
            _was_active = true;
        }
        else if (!ImGui::IsItemActive() && _was_active)
        {
            ed::EnableShortcuts(true);
            _was_active = false;
        }
        _builder->EndPort();
    }
    else if (Kind == PinKind::Output)
    {
        _builder->Output(ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, _alpha);
        DrawLabel();
        DrawIcon(_alpha);
        ImGui::PopStyleVar();
        _builder->EndOutput();
    }

    _is_linked = ed::PinHadAnyLinks(ID);
}

constexpr std::string_view AnyType = flow::TypeName_v<std::any>;

bool PortView::CanLink(const std::shared_ptr<PortView>& other) const noexcept
{
    if (_is_linked && Kind == PinKind::Input || !other || ID == other->ID || Kind == other->Kind ||
        NodeID == other->NodeID)
    {
        return false;
    }

    return Type() == other->Type() || Type() == AnyType || other->Type() == AnyType;
}

void PortView::DrawInput()
try
{
    if (!_input_field) return;

    if (auto new_data = _input_field->Draw())
    {
        OnSetInput(Key(), std::move(new_data));
    }
}
catch (const std::exception& e)
{
    SPDLOG_ERROR("Failed to draw input for pin: {0}", e.what());
}

void PortView::DrawLabel()
{
    if (!_show_label) return;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(std::string{Name()}.c_str());
}

void PortView::DrawIcon(float alpha) { ::flow::ui::DrawPinIcon(*this, IsLinked(), static_cast<int>(alpha * 255)); }

void PortView::ShowLinkable(const std::shared_ptr<PortView>& new_link_pin)
{
    _alpha = ImGui::GetStyle().Alpha;
    if (new_link_pin && !CanLink(new_link_pin))
    {
        if (new_link_pin.get() == this) return;

        _alpha *= (48.0f / 255.0f);
    }
}

FLOW_UI_NAMESPACE_END
