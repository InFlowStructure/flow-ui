// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"

#include <flow/core/NodeData.hpp>

FLOW_UI_SUBNAMESPACE_START(widgets)

/**
 * @brief Input field interface class.
 */
class InputInterface : public Widget
{
  public:
    virtual ~InputInterface() = default;

    /**
     * @brief Gets a node data pointer of a newly entered value.
     * @returns A new node data pointer containing the newly entered value.
     */
    virtual flow::SharedNodeData GetData() noexcept = 0;
};

/**
 * @brief Input field widget
 */
template<typename T>
class Input : public InputInterface
{
  public:
    Input(std::string name, const T& initial_value);

    virtual ~Input() = default;

    virtual void operator()() noexcept override;

    /**
     * @brief Gets a node data pointer of a newly entered value.
     * @returns A new node data pointer containing the newly entered value.
     */
    virtual flow::SharedNodeData GetData() noexcept override
    {
        auto d = std::move(_data);
        _data  = nullptr;
        return d;
    }

    /**
     * @brief Gets the vale that is currently entered into the input field.
     * @returns The current value in the input field.
     */
    virtual const T& GetValue() const noexcept { return _value; }

  private:
    std::string _name;
    T _value;
    flow::SharedNodeData _data;
};

FLOW_UI_SUBNAMESPACE_END
