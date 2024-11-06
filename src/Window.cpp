// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Window.hpp"

FLOW_UI_NAMESPACE_START

Window::Window(std::string name, std::string dockspace_name)
    : _name{std::move(name)}, _dockspace_name{std::move(dockspace_name)}
{
}

FLOW_UI_NAMESPACE_END
