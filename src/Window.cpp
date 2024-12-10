// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Window.hpp"

#include "widgets/Text.hpp"

#include <imgui.h>

FLOW_UI_NAMESPACE_START

Window::Window(std::string name) : _name{std::move(name)} {}

void Window::Draw()
{
    widgets::Text(
        "Nothing to show",
        widgets::Text::Alignment{widgets::Text::HorizontalAlignment::Centre, widgets::Text::VerticalAlignment::Centre},
        Colour(175, 175, 175))();
}

FLOW_UI_NAMESPACE_END
