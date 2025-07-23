// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Window.hpp"

#include "widgets/Text.hpp"

#include <imgui.h>

FLOW_UI_NAMESPACE_BEGIN

Window::Window(std::string name) : _name{std::move(name)} {}

void Window::Draw()
{
    widgets::Text("Nothing to show")
        .SetColour(Colour(175, 175, 175))
        .SetAlignment(widgets::Text::HorizontalAlignment::Centre, widgets::Text::VerticalAlignment::Middle)
        .Draw();
}

FLOW_UI_NAMESPACE_END
