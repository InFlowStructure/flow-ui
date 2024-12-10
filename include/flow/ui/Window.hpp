// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <string>

FLOW_UI_NAMESPACE_START

struct DefaultDockspaces
{
    static inline const std::string Main     = "MainDockSpace";
    static inline const std::string Property = "PropertySpace";
    static inline const std::string Misc     = "MiscSpace";
};

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
