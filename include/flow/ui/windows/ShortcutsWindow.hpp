#pragma once

#include "Core.hpp"
#include "Window.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Event.hpp>
#include <flow/core/Graph.hpp>
#include <flow/core/NodeFactory.hpp>

FLOW_UI_NAMESPACE_START

class ShortcutsWindow : public Window
{
  public:
    ShortcutsWindow();
    virtual ~ShortcutsWindow() = default;

    virtual void Draw() override;
};

FLOW_UI_NAMESPACE_END
