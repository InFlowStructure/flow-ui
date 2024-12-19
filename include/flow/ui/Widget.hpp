#pragma once

#include "Core.hpp"

FLOW_UI_NAMESPACE_START

/**
 * @brief Abstract base widget class.
 */
class Widget
{
  public:
    virtual ~Widget() = default;

    /**
     * @brief Drawing function for widgets.
     */
    virtual void operator()() noexcept = 0;
};

FLOW_UI_NAMESPACE_END
