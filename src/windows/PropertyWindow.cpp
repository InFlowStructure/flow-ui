// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "PropertyWindow.hpp"

#include <flow/ui/widgets/PropertyTree.hpp>
#include <flow/ui/widgets/Text.hpp>
#include <flow/ui/windows/GraphWindow.hpp>
#include <imgui.h>
#include <imgui_node_editor.h>

#include <array>
#include <set>
#include <string>
#include <string_view>

FLOW_UI_NAMESPACE_BEGIN

using namespace ax;
namespace ed = ax::NodeEditor;

PropertyWindow::PropertyWindow() : Window(PropertyWindow::Name) {}

void PropertyWindow::Draw()
{
    auto graph = _graph.lock();
    if (!GetEditorContext() || !graph)
    {
        return Window::Draw();
    }

    ed::SetCurrentEditor(std::bit_cast<ed::EditorContext*>(GetEditorContext().get()));

    std::array<ed::NodeId, 256> selected_ids;
    auto result = ed::GetSelectedNodes(selected_ids.data(), 256);

    if (result == 0)
    {
        widgets::Text("Select one or more nodes")
            .SetColour(Colour(175, 175, 175))
            .SetAlignment(widgets::Text::HAlignment::Centre, widgets::Text::VAlignment::Middle)
            .Draw();
        return;
    }

    std::set<ed::NodeId> ids(selected_ids.begin(), std::next(selected_ids.begin(), result));

    graph->Visit([&](auto& node) {
        if (!ids.contains(std::hash<flow::UUID>{}(node->ID()))) return;

        const std::string node_tree_name = node->GetName() + "##" + std::string(node->ID());
        widgets::PropertyTree properties(node_tree_name, 2);

        const auto make_port_data_property = [&](const auto& port) -> std::vector<std::shared_ptr<flow::ui::Widget>> {
            return {
                std::make_shared<widgets::Text>("Type"),
                std::make_shared<widgets::Text>(std::string{port->GetDataType()}),
                std::make_shared<widgets::Text>("Value"),
                std::make_shared<widgets::Text>(port->GetData() ? port->GetData()->ToString() : "None"),
            };
        };

        for (const auto& [key, input] : node->GetInputPorts())
        {
            const std::string key_name{std::string_view(key)};
            properties.AddProperty(key_name, make_port_data_property(input), "Inputs");
        }

        for (const auto& [key, output] : node->GetOutputPorts())
        {
            const std::string key_name{std::string_view(key)};
            properties.AddProperty(key_name, make_port_data_property(output), "Outputs");
        }

        properties.Draw();
    });
}

FLOW_UI_NAMESPACE_END
