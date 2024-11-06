// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <string>

FLOW_UI_NAMESPACE_START

struct DefaultDockspaces
{
    static inline const std::string FLOW_UI_API Main     = "MainDockSpace";
    static inline const std::string FLOW_UI_API Property = "PropertySpace";
    static inline const std::string FLOW_UI_API Misc     = "MiscSpace";
};

class Window
{
  public:
    Window(std::string name, std::string dockspace_name);
    virtual ~Window() = default;

    const std::string& GetName() const { return _name; }
    const std::string& GetDockspaceName() const { return _dockspace_name; }

    virtual void Init() {}
    virtual void Teardown() {}
    virtual void Draw() = 0;

  protected:
    std::string _name;
    std::string _dockspace_name;
};

FLOW_UI_NAMESPACE_END
