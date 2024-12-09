// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/utilities/Builders.hpp"

#include <flow/core/Node.hpp>
#include <imgui_node_editor.h>

#include <memory>
#include <string_view>

FLOW_UI_NAMESPACE_START

namespace ed = ax::NodeEditor;
namespace widgets
{
class InputInterface;
}

enum class PinKind
{
    Output,
    Input
};

class NodeView;

class PortView
{
  public:
    using OnInputFunc = std::function<void(const flow::IndexableName&, flow::SharedNodeData)>;

    PortView(const ed::NodeId& node_id, std::shared_ptr<Port> port_data,
             const std::shared_ptr<class ViewFactory>& factory, OnInputFunc on_set_input, bool show_label = true);

    void Draw();

    void SetLinked(bool new_value) noexcept { _is_linked = new_value; }
    bool IsLinked() const noexcept { return _is_linked; }
    bool CanLink(const std::shared_ptr<PortView>& other) const noexcept;

    const flow::SharedNodeData& GetData() const noexcept { return _port->GetData(); }

    template<typename T>
    flow::TSharedNodeData<T> GetData() const noexcept
    {
        return CastNodeData<T>(GetData());
    }

    const flow::IndexableName& Key() const noexcept { return _port->GetKey(); }
    std::string_view Name() const noexcept { return _port->GetVarName(); }
    std::string_view Caption() const noexcept { return _port->GetCaption(); }
    std::string_view Type() const noexcept { return _port->GetDataType(); }
    Colour GetColour() const noexcept { return GetStyle().GetTypeColour(Type()); }

    void SetBuilder(std::shared_ptr<NodeBuilder> builder) noexcept { _builder = std::move(builder); }

    void ShowLinkable(const std::shared_ptr<PortView>& new_link_pin);

    void SetShowLabel(bool show) { _show_label = show; }

  protected:
    void DrawInput();

  private:
    void DrawLabel();
    void DrawIcon(float alpha);

  public:
    ed::PinId ID;
    const ed::NodeId& NodeID;
    PinKind Kind = PinKind::Input;

    OnInputFunc OnSetInput;

  private:
    std::shared_ptr<Port> _port;
    std::shared_ptr<widgets::InputInterface> _input_field;

    bool _show_label = true;
    bool _is_linked  = false;
    bool _was_active = false;
    float _alpha     = 1.f;

    std::shared_ptr<NodeBuilder> _builder;
};
FLOW_UI_NAMESPACE_END
