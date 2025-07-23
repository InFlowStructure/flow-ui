#pragma once

#include "Core.hpp"
#include "Window.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Event.hpp>
#include <flow/core/Graph.hpp>
#include <flow/core/NodeFactory.hpp>

FLOW_UI_NAMESPACE_BEGIN

class NodeExplorerWindow : public Window
{
  public:
    NodeExplorerWindow(std::shared_ptr<Env> env);
    virtual ~NodeExplorerWindow() = default;

    virtual void Draw() override;

    void SetActiveGraph(std::shared_ptr<Graph> graph) { _active_graph = std::move(graph); }

  private:
    void DrawPopupCategory(const std::string& category, const flow::CategoryMap& registered_nodes);

  public:
    Event<const std::string&, const std::string&> OnSelection;

  private:
    std::shared_ptr<Env> _env;
    std::shared_ptr<Graph> _active_graph;
    std::string node_lookup;
};

FLOW_UI_NAMESPACE_END
