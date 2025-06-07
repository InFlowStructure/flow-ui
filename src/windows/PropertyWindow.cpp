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

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

constexpr Colour empty_property_text_colour = Colour(175, 175, 175);

struct CentredText : public widgets::Text
{
    CentredText(const std::string& text, const Colour& c)
        : Text(text, c, {widgets::Text::HorizontalAlignment::Centre, widgets::Text::VerticalAlignment::Centre})
    {
    }

    virtual ~CentredText() = default;
};

PropertyWindow::PropertyWindow(std::shared_ptr<flow::Env> env) : Window(PropertyWindow::Name), _env{env} {}

struct NodeProperty
{
    std::string Name;
    flow::SharedNode Node;
};

void PropertyWindow::Draw()
{
    auto env = _env.lock();
    if (!env) return;

    auto graph = _graph.lock();
    if (!GetEditorContext() || !graph)
    {
        CentredText("Nothing to show", empty_property_text_colour)();
        return;
    }

    ed::SetCurrentEditor(std::bit_cast<ed::EditorContext*>(GetEditorContext().get()));

    std::array<ed::NodeId, 256> selected_ids;
    auto result = ed::GetSelectedNodes(selected_ids.data(), 256);

    if (result == 0)
    {
        CentredText("Select one or more nodes", empty_property_text_colour)();
        return;
    }

    std::set<ed::NodeId> ids(selected_ids.begin(), std::next(selected_ids.begin(), result));

    std::vector<flow::SharedNode> nodes;
    nodes.reserve(result);

    graph->Visit([&](auto& node) {
        if (!ids.contains(std::hash<flow::UUID>{}(node->ID()))) return;
        nodes.emplace_back(node);
    });

    for (auto& node : nodes)
    {
        std::string c_name = node->GetName() + "##" + std::to_string(std::hash<flow::UUID>{}(node->ID()));
        widgets::PropertyTree properties(c_name, 2);

        // Get and display node position and size
        ed::NodeId node_id = std::hash<flow::UUID>{}(node->ID());
        ImVec2 node_pos = ed::GetNodePosition(node_id);
        ImVec2 node_size = ed::GetNodeSize(node_id);
        
        // Add position property
        properties.AddProperty("Position", {
            std::make_shared<widgets::Text>("X"),
            std::make_shared<widgets::Text>(std::to_string(static_cast<int>(node_pos.x))),
            std::make_shared<widgets::Text>("Y"),
            std::make_shared<widgets::Text>(std::to_string(static_cast<int>(node_pos.y)))
        }, "Node Info");
        
        // Add size property
        properties.AddProperty("Size", {
            std::make_shared<widgets::Text>("Width"),
            std::make_shared<widgets::Text>(std::to_string(static_cast<int>(node_size.x))),
            std::make_shared<widgets::Text>("Height"),
            std::make_shared<widgets::Text>(std::to_string(static_cast<int>(node_size.y)))
        }, "Node Info");

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

        properties();
    }
}

FLOW_UI_NAMESPACE_END
