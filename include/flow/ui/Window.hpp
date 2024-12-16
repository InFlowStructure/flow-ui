// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <string>

FLOW_UI_NAMESPACE_START

static inline const std::string DefaultDockspace = "MainDockSpace";

class Window
{
  public:
    Window(std::string name);
    virtual ~Window() = default;

    const std::string& GetName() const { return _name; }

    virtual void Init() {}
    virtual void Teardown() {}
    virtual void Draw();

  protected:
    std::string _name;
};

FLOW_UI_NAMESPACE_END
