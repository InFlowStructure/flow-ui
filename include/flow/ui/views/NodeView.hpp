// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "LinkView.hpp"
#include "flow/ui/Core.hpp"
#include "flow/ui/utilities/Builders.hpp"
#include "flow/ui/utilities/Helpers.hpp"

#include <flow/core/Node.hpp>
#include <imgui_node_editor.h>

#include <deque>
#include <set>
#include <string_view>
#include <type_traits>

FLOW_UI_NAMESPACE_START

namespace ed = ax::NodeEditor;

class Link;
class PortView;

class GraphItemView
{
  public:
    GraphItemView(ed::NodeId id) : _id{std::move(id)} {}
    virtual ~GraphItemView();

    virtual void Draw() = 0;

    virtual void ShowLinkables(const std::shared_ptr<PortView>&) {}

    const ed::NodeId& ID() const { return _id; }

  protected:
    ed::NodeId _id;
};

class NodeView : public GraphItemView
{
  public:
    NodeView(flow::SharedNode node, ImColor color = ImColor(40, 75, 99));
    virtual ~NodeView() = default;

    void Draw() override;

    void ShowLinkables(const std::shared_ptr<PortView>& new_link_port) override;

  public:
    std::string Name;
    std::vector<std::shared_ptr<PortView>> Inputs;
    std::vector<std::shared_ptr<PortView>> Outputs;
    ImColor Color;

    flow::SharedNode Node;

  protected:
    std::shared_ptr<NodeBuilder> _builder = std::make_shared<NodeBuilder>();
    bool _received_error                  = false;
};

class SimpleNodeView : public NodeView
{
  public:
    SimpleNodeView(flow::SharedNode node);
    virtual ~SimpleNodeView() = default;

    void Draw() override;
};

class CommentView : public GraphItemView
{
  public:
    CommentView(ImVec2 size, std::string_view name = "Comment");
    virtual ~CommentView() = default;

    void Draw() override;

  private:
    bool _edit = false;

  public:
    std::string Name;
    ImVec2 Size;
};

class RerouteNodeView : public GraphItemView
{
  public:
    RerouteNodeView(const ed::PinId& start_pin, const ed::PinId& end_pin);
    virtual ~RerouteNodeView() = default;

    void Draw() override;

  public:
    ed::PinId InputPinID;
    std::set<ed::PinId> OutputIDs;

    ImColor Colour;

    ed::PinId _input_id;
    ed::PinId _output_id;
};
FLOW_UI_NAMESPACE_END
