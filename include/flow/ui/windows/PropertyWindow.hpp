// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Window.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Graph.hpp>

#include <memory>
#include <string>

FLOW_UI_NAMESPACE_START

class PropertyWindow : public Window
{
  public:
    PropertyWindow(std::shared_ptr<flow::Env> env);
    virtual ~PropertyWindow() = default;

    virtual void Draw() override;

    void SetCurrentGraph(std::shared_ptr<flow::Graph> graph) { _graph = graph; }

    static inline const std::string Name = "Properties";

  private:
    std::weak_ptr<flow::Env> _env;
    std::weak_ptr<flow::Graph> _graph;
};

FLOW_UI_NAMESPACE_END
