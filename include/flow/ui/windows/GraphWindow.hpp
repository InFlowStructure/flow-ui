// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"
#include "flow/ui/Window.hpp"
#include "flow/ui/views/NodeView.hpp"

#include <flow/core/Graph.hpp>
#include <flow/core/NodeFactory.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

FLOW_UI_NAMESPACE_START

using json = nlohmann::json;

class ContextMenu : public Widget
{
  public:
    ContextMenu(std::shared_ptr<NodeFactory> factory) : _factory(std::move(factory)) {}

    virtual ~ContextMenu() = default;

    virtual void operator()() noexcept override;

  private:
    void DrawPopupCategory(const std::string& category, const flow::CategoryMap& registered_nodes);

  public:
    Event<const std::string&, const std::string&> OnSelection;

  private:
    std::shared_ptr<NodeFactory> _factory;
    std::string node_lookup;
    bool is_focused = false;
};

/**
 * @brief Graph editor window for creating flows.
 */
class GraphWindow : public Window
{
    /**
     * @brief Type of input action.
     */
    enum class ActionType : std::uint8_t
    {
        Create = 1 << 0,
        Delete = 1 << 1,
    };

    friend constexpr ActionType operator&(const ActionType& a, const ActionType& b);

    /**
     * @brief Type of action in the graph.
     */
    struct Action
    {
        /// Action type whether a graph item was created or deleted.
        ActionType Type;

        /// The information that was created/deleted.
        json Info;
    };

  public:
    /**
     * @brief Constructs a graph editor window.
     * @param graph The flow graph editor window.
     */
    GraphWindow(std::shared_ptr<flow::Graph> graph);

    virtual ~GraphWindow();

    /**
     * @brief Sets this graph window as the active graph context.
     */
    void SetCurrentGraph();

    /**
     * @brief Renders the flow graph editor window.
     */
    virtual void Draw() override;

    /**
     * @brief Get the actual computation flow graph.
     * @returns A pointer to the flow Graph.
     */
    const std::shared_ptr<flow::Graph>& GetGraph() const { return _graph; }

    /**
     * @brief Gets a pointer to the graph editor context.
     * @returns THe editor context pointer.
     */
    const std::unique_ptr<EditorContext>& GetEditorContext() const { return _editor_ctx; }

    /**
     * @brief Gets whether the graph window is active or not.
     * @returns true if the graph window is the active/focused window, false otherwise.
     */
    bool IsActive() const { return _active; }

    /**
     * @brief Gets whether or not the graph window is open.
     * @returns true if the graph window is open, false otherwise.
     */
    bool IsOpen() const { return _open; }

    /**
     * @brief Creates the flow JSON to be saved.
     * @returns The flow JSON that was saved.
     */
    json SaveFlow();

    /**
     * @brief Creates a flow graph from loaded JSON.
     * @param j The JSON to read the flow graph from.
     */
    void LoadFlow(const json& j);

    /**
     * @brief Undo last input command.
     */
    void UndoChange();

    /**
     * @brief Redo the last change that was undone.
     */
    void RedoChange();

    /**
     * @brief Gets a copy of the selected portion of the graph and creates the appropriate JSON.
     * @returns The JSON representation of the selected portion of the graph.
     */
    json CopySelection();

    /**
     * @brief Adds nodes and connections to the graph from a given JSON object.
     * @param flow_json The JSON object to create nodes from.
     */
    void CreateNodesAction(const json& flow_json);

    /**
     * @brief Delete nodes and connections fomr the graph based on the given action info.
     * @param action The action and JSON info to delete.
     */
    void DeleteNodesAction(Action& action);

    /**
     * @brief Adds a comment to the graph around the selected nodes in the graph.
     * @note If no nodes are selected, no comment is created.
     */
    void CreateComment();

    /**
     * @brief Gets a node view pointer from the graph based on a UUID hash.
     * @param id The ID of the node view to get.
     * @returns The found node view, nullptr otherwise.
     */
    std::shared_ptr<NodeView> FindNode(std::uint64_t id) const;

    /**
     * @brief Gets a connection view by its ID.
     * @param id The ID of the connection view.
     * @returns A reference to the requested ConncectionView.
     */
    ConnectionView& FindConnection(std::uint64_t id);

    /**
     * @brief Gets a PortView by its UUID hash.
     * @param id The ID of the PortView.
     * @returns The shared pointer to the port view, nullptr otherwise.
     */
    std::shared_ptr<PortView> FindPort(std::uint64_t id) const;

    /**
     * @brief Gets a comment by its NodeView ID.
     * @param id THe ID of the comment.
     * @returns THe shared pointer to the comment, false otherwise.
     */
    std::shared_ptr<CommentView> FindComment(std::uint64_t id) const;

    /**
     * @brief Marks the window as dirty/modified.
     * @param new_value true for when the window has been modified, false otherwise.
     */
    void MarkDirty(bool new_value) { _dirty = new_value; }

  private:
    void EndDraw();

    const std::shared_ptr<flow::Env>& GetEnv() const { return _graph->GetEnv(); }

    void DeleteNode(std::uint64_t id);
    bool DeleteLink(std::uint64_t id);
    void ShowLinkFlowing(const flow::UUID& node_id, const IndexableName& key);

    void CreateItems();
    void CleanupDeadItems();

    void OnLoadNode(const flow::SharedNode& node, const json& position_json);
    void OnLoadConnection(const flow::SharedConnection& connection);

    flow::SharedNode CreateNode(const std::string& class_name, const std::string& display_name);

  private:
    mutable std::mutex _mutex;
    std::unique_ptr<EditorContext> _editor_ctx;
    std::shared_ptr<flow::Graph> _graph;

    std::unordered_map<std::uint64_t, std::shared_ptr<GraphItemView>> _item_views;
    std::unordered_map<std::uint64_t, ConnectionView> _links;

    std::stack<Action> _undo_history;
    std::stack<Action> _redo_history;

    std::shared_ptr<PortView> _new_node_link_pin = nullptr;
    std::shared_ptr<PortView> _new_link_pin      = nullptr;

    ContextMenu _node_creation_context_menu;

    struct
    {
        float x = 0.f;
        float y = 0.f;
    } _open_popup_position;

    bool _active             = true;
    bool _open               = true;
    bool _dirty              = true;
    bool _create_new_node    = false;
    bool _get_popup_location = false;
};

FLOW_UI_NAMESPACE_END
