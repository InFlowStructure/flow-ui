// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "ConnectionView.hpp"
#include "flow/ui/Core.hpp"

#include <flow/core/Node.hpp>

#include <deque>
#include <set>
#include <string_view>
#include <type_traits>

FLOW_UI_NAMESPACE_START

namespace utility
{
class NodeBuilder;
}

class ConnectionView;
class PortView;

/**
 * @brief Abstract base class for items drawn on the graph.
 */
class GraphItemView
{
  public:
    /**
     * @brief Constructs a graph item with an ID.
     * @param id The ID of the graph item.
     */
    GraphItemView(std::uint64_t id) : _id{std::move(id)} {}

    virtual ~GraphItemView();

    /**
     * @brief Render the item to the graph.
     */
    virtual void Draw() = 0;

    /**
     * @brief Show items that can be be connected to by the given port.
     * @param port The port to check against.
     */
    virtual void ShowConnectables(const std::shared_ptr<PortView>& port);

    /**
     * @brief Gets the ID of the graph item.
     * @returns THe UUID hash of the graph item.
     */
    constexpr const std::uint64_t& ID() const noexcept { return _id; }

  protected:
    std::uint64_t _id;
};

/**
 * @brief The default view of a Node.
 */
class NodeView : public GraphItemView
{
  public:
    /**
     * @brief Constructs a node view for a given node.
     *
     * @param node The node to represent.
     * @param header_colour The colour of the node header.
     */
    NodeView(flow::SharedNode node, Colour header_colour = Colour(40, 75, 99));

    virtual ~NodeView() = default;

    /**
     * @brief Render the node to the graph.
     */
    void Draw() override;

    /**
     * @brief Shows ports that can connect to the given port, and hides ports that cannot.
     * @param port The port to check other ports against.
     */
    void ShowConnectables(const std::shared_ptr<PortView>& port) override;

  public:
    /// The name of the Node.
    std::string Name;

    /// The input port viewss of the node.
    std::vector<std::shared_ptr<PortView>> Inputs;

    /// The output port views of the node.
    std::vector<std::shared_ptr<PortView>> Outputs;

    /// The colour of the header.
    Colour HeaderColour;

    /// The node that is being represented.
    flow::SharedNode Node;

  protected:
    std::shared_ptr<utility::NodeBuilder> _builder;
    bool _received_error = false;
};

/**
 * @brief Simple node view representation that has no header.
 */
class SimpleNodeView : public NodeView
{
  public:
    /**
     * @brief Constructs a simple node view.
     * @param node The node being represented.
     */
    SimpleNodeView(flow::SharedNode node);

    virtual ~SimpleNodeView() = default;

    /**
     * @brief Render the node to the graph.
     */
    void Draw() override;
};

/**
 * @brief Graph editor comment.
 */
class CommentView : public GraphItemView
{
  public:
    /**
     * @brief Comment size.
     */
    struct CommentSize
    {
        /// The width of the comment.
        float Width;

        /// The height of the comment.
        float Height;
    };

    /**
     * @brief Constructs a comment on the graph.
     * @param size The size of the comment on the graph.
     * @param title The title of the comment.
     */
    CommentView(CommentSize size, std::string_view title = "Comment");

    virtual ~CommentView() = default;

    /**
     * @brief Render the comment on the graph.
     */
    void Draw() override;

  public:
    /// The title of the comment.
    std::string Name;

    /// The size of the comment.
    CommentSize Size;

  private:
    bool _edit = false;
};

FLOW_UI_NAMESPACE_END
