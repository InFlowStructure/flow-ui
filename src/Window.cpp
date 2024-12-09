// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Window.hpp"

#include <imgui.h>

FLOW_UI_NAMESPACE_START

Window::Window(std::string name) : _name{std::move(name)} {}

FLOW_UI_NAMESPACE_END
