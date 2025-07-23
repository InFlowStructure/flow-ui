// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "PortView.hpp"

#include "ConnectionView.hpp"
#include "ViewFactory.hpp"
#include "utilities/NodeBuilder.hpp"
#include "utilities/Conversions.hpp"
#include "utilities/Widgets.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/NodeData.hpp>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <map>
#include <vector>

FLOW_UI_NAMESPACE_BEGIN

using namespace ax;
namespace ed = ax::NodeEditor;

namespace
{
PortIconType GetIconType(std::string_view type)
{
    if (type.find("*") != std::string_view::npos || type.find("unique_ptr") != std::string_view::npos ||
        type.find("&") != std::string_view::npos)
    {
        return GetStyle().PortShapes.Ref;
    }

    return GetStyle().PortShapes.Default;
}

void DrawPinIcon(const PortView& pin, bool connected, int alpha)
{
    auto colour = pin.GetColour();
    colour.A    = static_cast<std::uint8_t>(alpha);
    widgets::Icon(ImVec2(24.f, 24.f), GetIconType(pin.GetType()), connected, utility::to_ImColor(colour),
                  ImColor(32, 32, 32, alpha));
}
} // namespace

PortView::PortView(PortType type, const std::uint64_t& node_id, SharedPort port_data)
    : ID(std::hash<flow::UUID>{}({})), NodeViewID(node_id), Name(port_data->GetVarName()), _port{std::move(port_data)},
      _show_label{_port->GetKey() != flow::IndexableName::None}, Type(type)
{
}

void PortView::Draw(const std::shared_ptr<utility::NodeBuilder>& builder)
{
    if (Type == PortType::Input)
    {
        builder->Input(ID);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, _alpha);
        DrawIcon(_alpha);
        DrawLabel();
        ImGui::PopStyleVar();

        if (!IsConnected())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
            DrawInput();
            ImGui::PopStyleVar();
        }

        builder->EndInput();

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
    }
    else if (Type == PortType::Output)
    {
        builder->Output(ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, _alpha);
        ImGui::Spring(1);
        DrawLabel();
        DrawIcon(_alpha);
        ImGui::PopStyleVar();
        builder->EndOutput();
    }
}

constexpr std::string_view AnyType = flow::TypeName_v<std::any>;

bool PortView::CanLink(const std::shared_ptr<PortView>& other) const noexcept
{
    if (IsConnected() && Type == PortType::Input || !other || ID == other->ID || Type == other->Type ||
        NodeViewID == other->NodeViewID)
    {
        return false;
    }

    return GetType() == other->GetType() || GetType() == AnyType || other->GetType() == AnyType;
}

void PortView::DrawInput()
try
{
    if (!_input_field)
    {
        return;
    }

    _input_field->Draw();

    if (auto new_data = _input_field->GetData())
    {
        OnSetInput(GetKey(), std::move(new_data));
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
    ImGui::TextUnformatted(std::string{Name.substr(0, Name.find("##"))}.c_str());
}

void PortView::DrawIcon(float alpha) { ::flow::ui::DrawPinIcon(*this, IsConnected(), static_cast<int>(alpha * 255)); }

void PortView::ShowConnectable(const std::shared_ptr<PortView>& new_link_pin)
{
    _alpha = ImGui::GetStyle().Alpha;
    if (new_link_pin && new_link_pin.get() != this && !CanLink(new_link_pin))
    {
        _alpha *= 0.2f;
    }
}

FLOW_UI_NAMESPACE_END
