#pragma once

#include "Core.hpp"

FLOW_UI_NAMESPACE_START

class Widget
{
  public:
    virtual ~Widget() = default;

    virtual void operator()() noexcept = 0;
};

FLOW_UI_NAMESPACE_END
