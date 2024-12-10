// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Window.hpp"
#include "flow/ui/utilities/Helpers.hpp"
#include "flow/ui/views/NodeView.hpp"

#include <flow/core/Graph.hpp>
#include <flow/core/NodeFactory.hpp>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

FLOW_UI_NAMESPACE_START

class GraphWindow : public Window
{
    enum class ActionType : std::uint8_t
    {
        Create = 1 << 0,
        Delete = 1 << 1,
    };

    friend constexpr ActionType operator&(const ActionType& a, const ActionType& b);

    struct Action
    {
        ActionType Type;
        json Info;
    };

  public:
    GraphWindow(std::shared_ptr<flow::Graph> graph);
    virtual ~GraphWindow();

    void SetCurrentGraph();

    virtual void Draw() override;

    std::shared_ptr<flow::Graph>& GetGraph() { return _graph; }
    const std::shared_ptr<flow::Graph>& GetGraph() const { return _graph; }

    ed::EditorContext* GetEditorContext() const { return _editor_ctx; }

    bool IsActive() const { return _active; }
    bool IsOpen() const { return _open; }

    json SaveFlow();
    void LoadFlow(const json& j);

    void UndoChange();
    void RedoChange();

    json CopySelection();
    void CreateNodesAction(const json& flow_json);
    void DeleteNodesAction(Action& action);

    void CreateComment();

    std::shared_ptr<NodeView> FindNode(ed::NodeId id) const;
    Link& FindLink(ed::LinkId id);
    std::shared_ptr<PortView> FindPin(ed::PinId id) const;
    std::shared_ptr<CommentView> FindComment(ed::NodeId id) const;

    void MarkDirty(bool new_value) { _dirty = new_value; }

  protected:
    void EndDraw();

    const std::shared_ptr<flow::Env>& GetEnv() const { return _graph->GetEnv(); }

    void DeleteNode(ed::NodeId id);
    bool DeleteLink(ed::LinkId id);
    void ShowLinkFlowing(const flow::UUID& node_id, const IndexableName& key);

    void CreateItems();
    void CleanupDeadItems();
    void ShowNodeContextMenu();

    void OnLoadNode(const flow::SharedNode& node, const json& position_json);
    void OnLoadConnection(const flow::SharedConnection& connection);

    ed::Detail::EditorContext* GetEditorDetailContext() const;

    flow::SharedNode CreateNode(const std::string& class_name, const std::string& display_name);

    void DrawPopupCategory(const std::string& category, const flow::CategoryMap& registered_nodes);

  private:
    ed::EditorContext* _editor_ctx;
    std::shared_ptr<flow::Graph> _graph;

    std::unordered_map<ed::NodeId, std::shared_ptr<GraphItemView>> _item_views;
    std::unordered_map<ed::LinkId, Link> _links;

    std::stack<Action> _undo_history;
    std::stack<Action> _redo_history;

    std::shared_ptr<PortView> _new_node_link_pin = nullptr;
    std::shared_ptr<PortView> _new_link_pin      = nullptr;

    std::string node_lookup;

    ImVec2 _open_popup_position = ImVec2(0, 0);

    bool _active             = true;
    bool _open               = true;
    bool _dirty              = true;
    bool _create_new_node    = false;
    bool _get_popup_location = false;
};

FLOW_UI_NAMESPACE_END
