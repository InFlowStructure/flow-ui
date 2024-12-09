// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"

#include <flow/core/NodeData.hpp>

FLOW_UI_SUBNAMESPACE_START(widgets)

class InputInterface : public Widget
{
  public:
    virtual flow::SharedNodeData GetData() noexcept = 0;
};

/**
 * Input field widget
 */
template<typename T>
class Input : public InputInterface
{
  public:
    Input(std::string name, const T& initial_value);

    virtual void operator()() noexcept override;

    virtual flow::SharedNodeData GetData() noexcept
    {
        auto d = std::move(_data);
        _data  = nullptr;
        return d;
    }

  private:
    std::string _name;
    T _value;
    flow::SharedNodeData _data;
};

FLOW_UI_SUBNAMESPACE_END
